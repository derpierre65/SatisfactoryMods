const fs = require('fs');
const path = require('path');

const oreValues = {
    "/Game/FactoryGame/Resource/RawResources/OreIron/Desc_OreIron.Desc_OreIron_C": 1,
    "/Game/FactoryGame/Resource/RawResources/Stone/Desc_Stone.Desc_Stone_C": 2,
    "/Game/FactoryGame/Resource/RawResources/OreCopper/Desc_OreCopper.Desc_OreCopper_C": 3,
    "/Game/FactoryGame/Resource/RawResources/Coal/Desc_Coal.Desc_Coal_C": 3,
    "/Game/FactoryGame/Resource/RawResources/OreGold/Desc_OreGold.Desc_OreGold_C": 7,
    "/Game/FactoryGame/Resource/RawResources/OreBauxite/Desc_OreBauxite.Desc_OreBauxite_C": 8,
    "/Game/FactoryGame/Resource/RawResources/Sulfur/Desc_Sulfur.Desc_Sulfur_C": 11,
    "/Game/FactoryGame/Resource/RawResources/RawQuartz/Desc_RawQuartz.Desc_RawQuartz_C": 15,
    "/Game/FactoryGame/Resource/RawResources/SAM/Desc_SAM.Desc_SAM_C": 20,
    "/Game/FactoryGame/Resource/RawResources/OreUranium/Desc_OreUranium.Desc_OreUranium_C": 35,

    "/Game/FactoryGame/Resource/Parts/Rubber/Desc_Rubber.Desc_Rubber_C": 60,
    "/Game/FactoryGame/Resource/Parts/Plastic/Desc_Plastic.Desc_Plastic_C": 75,
};

const baseEMCPerSinkPoint = 128;

for (const ore of Object.keys(oreValues)) {
    oreValues[ore] *= baseEMCPerSinkPoint;
}

const considerRecipes = [
    'Recipe_Alternate_EnrichedCoal_C',
    'Recipe_TimeCrystal_C',
    'Recipe_FicsiteIngot_Iron_C',
    'Recipe_DarkEnergy_C',
];

const ignoreItems = [
    '/Game/FactoryGame/Resource/Parts/AlienProtein/Desc_AlienProtein.Desc_AlienProtein_C',
    '/Game/FactoryGame/Resource/Parts/AlienDNACapsule/Desc_AlienDNACapsule.Desc_AlienDNACapsule_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_HogParts.Desc_HogParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_HatcherParts.Desc_HatcherParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_StingerParts.Desc_StingerParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_SpitterParts.Desc_SpitterParts_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_CrystalShard.Desc_CrystalShard_C',
];

const emcValues = {
    ...oreValues,
    "/Game/FactoryGame/Resource/Parts/GenericBiomass/Desc_Leaves.Desc_Leaves_C": 1,
    '/Game/FactoryGame/Resource/RawResources/Water/Desc_Water.Desc_Water_C': 0,
    '/Game/FactoryGame/Resource/Parts/Alumina/Desc_AluminaSolution.Desc_AluminaSolution_C': 544,
    '/Game/FactoryGame/Resource/Environment/Berry/Desc_Berry.Desc_Berry_C': 4096,
    '/Game/FactoryGame/Resource/Environment/Nut/Desc_Nut.Desc_Nut_C': 1024,

    "/Game/FactoryGame/Resource/Parts/QuantumEnergy/Desc_QuantumEnergy.Desc_QuantumEnergy_C": 64,
};

const calculateEmcValue = {
    '/Game/FactoryGame/Resource/RawResources/NitrogenGas/Desc_NitrogenGas.Desc_NitrogenGas_C': () => {
        const requiredNitrogen = 4;
        const targetPointValue = 312
        
        const tankPrice = emcValues["/Game/FactoryGame/Resource/Parts/GasTank/Desc_GasTank.Desc_GasTank_C"];
        if (!tankPrice) {
            return false;
        }
        
        return targetPointValue * baseEMCPerSinkPoint / requiredNitrogen;
    },
};

fetch('http://localhost:8081/getRecipes').then(response => response.json()).then(data => {
    const alternateRecipes = [];
    const recipesLeft = [];
    for (const recipe of data) {
        if (['N/A', 'FICSMAS'].includes(recipe.Category)) continue;
        if (recipe.Name.startsWith('Alternate:') && !considerRecipes.includes(recipe.ClassName)) {
            alternateRecipes.push(recipe);
            continue;
        }

        if (recipe.Ingredients.length === 0) {
            console.log('Ignore recipe with no ingredients: ', recipe.Name)
            continue;
        }

        if (recipe.ProducedIn.includes('BP_WorkshopComponent_C')) {
            continue;
        }

        if (recipe.ProducedIn.includes('Build_Converter_C') && !considerRecipes.includes(recipe.ClassName)) {
            continue;
        }

        recipesLeft.push(recipe);
    }

    recipesLeft.sort((a, b) => {
        return a.Products.length - b.Products.length;
    });

    function getRecipeText(recipe) {
        return [
            recipe.Name,
            '||',
            recipe.Ingredients.map((ingredient) => ingredient.Amount + 'x ' + ingredient.Name + `(${emcValues[ingredient.ClassPath] * ingredient.Amount})`).join(', '),
            '=',
            recipe.Products.map((ingredient) => ingredient.Amount + 'x ' + ingredient.Name).join(', '),
        ].join(' ');
    }

    function setEmcValue(item, value) {
        if (ignoreItems.includes(item)) {
            return;
        }

        if (item.includes('SpaceElevatorPart_')) {
            return;
        }

        emcValues[item] = Math.floor(value);
    }

    function calculateItemManually(classPath) {
        if ( !calculateEmcValue[classPath] ) {
            return;
        }

        let calculatedValue = calculateEmcValue[classPath]();
        if ( calculatedValue === false ) {
            return;
        }

        emcValues[classPath] = calculatedValue;
    }
    
    function calculateRecipes() {
        const recipes = recipesLeft.slice();
        const before = recipesLeft.length;
        for (const recipe of recipes) {
            const inputEmcValue = recipe.Ingredients.reduce((before, ingredient) => {
                calculateItemManually(ingredient.ClassPath);
                if (before === -1 || typeof emcValues[ingredient.ClassPath] === 'undefined') {
                    return -1;
                }

                return before + emcValues[ingredient.ClassPath] * ingredient.Amount;
            }, 0);
            const outputEmcValue = recipe.Products.reduce((before, product) => {
                calculateItemManually(product.ClassPath);
                if (before === -1 || typeof emcValues[product.ClassPath] === 'undefined') {
                    
                    return -1;
                }

                return before + emcValues[product.ClassPath] * product.Amount;
            }, 0);
            const productsWithoutEmc = recipe.Products.filter((product) => typeof emcValues[product.ClassPath] === 'undefined')
            const ingredientsWithoutEmc = recipe.Ingredients.filter((product) => typeof emcValues[product.ClassPath] === 'undefined')

            if (inputEmcValue > 0 && outputEmcValue > 0) {
                continue;
            }

            if (inputEmcValue < 0 && outputEmcValue < 0) {
                console.log(`Ignore ${recipe.Name} ${inputEmcValue} ${outputEmcValue}`, ingredientsWithoutEmc, productsWithoutEmc);
                continue;
            }

            if (inputEmcValue >= 0 && outputEmcValue < 0) {
                if (recipe.Products.length === 1) {
                    console.log('calculate by input')
                    setEmcValue(recipe.Products[0].ClassPath, inputEmcValue / recipe.Products[0].Amount);
                } else {
                    const productsEmcValue = recipe.Products
                        .filter((product) => typeof emcValues[product.ClassPath] !== 'undefined')
                        .reduce((before, product) => before + emcValues[product.ClassPath] * product.Amount, 0);
                    if (productsWithoutEmc.length > 1) {
                        console.log(`too many unknown products for ${recipe.Name}`);
                        continue;
                    }

                    const newProductPrice = (inputEmcValue - productsEmcValue) / productsWithoutEmc[0].Amount;

                    setEmcValue(productsWithoutEmc[0].ClassPath, newProductPrice);
                }
            } else if (outputEmcValue >= 0 && inputEmcValue < 0) {
                console.log('calculate by output')

                if (recipe.Ingredients.length === 1) {
                    setEmcValue(recipe.Ingredients[0].ClassPath, outputEmcValue / recipe.Products[0].Amount);
                } else {
                    console.log('Cannot calculate by output, multiple ingredients: ', getRecipeText(recipe))
                    continue;
                }
            } else {
                console.log('???', inputEmcValue, outputEmcValue);
                continue;
            }

            console.log(`analyzed ${recipe.Name}`);
            recipesLeft.splice(recipesLeft.indexOf(recipe), 1);
        }

        if (recipesLeft.length && before !== recipesLeft.length) {
            console.clear();
            calculateRecipes();
        } else {
            console.log('Missing Items:', recipesLeft.reduce((before, recipe) => {
                const items = [
                    ...recipe.Ingredients.filter(item => !emcValues[item.ClassPath]).map((item) => item.ClassPath),
                    ...recipe.Products.filter(item => !emcValues[item.ClassPath]).map((item) => item.ClassPath),
                ];
                for (const item of items) {
                    if (!before.includes(item)) {
                        before.push(item);
                    }
                }

                return before;
            }, []));

            const itemKeys = Object.keys(emcValues);
            itemKeys.sort((a, b) => emcValues[a] - emcValues[b]);
            const csvFile = [
                '----,ItemClass,EmcValue',
                ...itemKeys.map(item => {
                    const name = item.split('/').pop().replace(/\.Desc_.*$/, '').replace(/^Desc_/, '');

                    return `${name},"${item}",${Math.floor(emcValues[item])}`;
                }),
            ];

            console.log(csvFile.length + ' items with EMC value');
            let emcValuesCsv = 'M:\\Steam\\steamapps\\common\\Satisfactory\\FactoryGame\\Mods\\EquivalentExchange\\Export\\EmcValues.csv';
            fs.mkdirSync(path.dirname(emcValuesCsv), { recursive: true });
            fs.writeFileSync(emcValuesCsv, csvFile.join('\n'));
        }
    }

    calculateRecipes();
    // console.log(recipesLeft.length);
    // N/A
});