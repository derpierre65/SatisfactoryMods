const fs = require('fs');
const path = require('path');

const baseEMCPerSinkPoint = 128;
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
for (const ore of Object.keys(oreValues)) {
    oreValues[ore] *= baseEMCPerSinkPoint;
}
const emcValues = {
    ...oreValues,
    "/Game/FactoryGame/Resource/Parts/GenericBiomass/Desc_Leaves.Desc_Leaves_C": 1,
    '/Game/FactoryGame/Resource/RawResources/Water/Desc_Water.Desc_Water_C': 0,
    '/Game/FactoryGame/Resource/Parts/Alumina/Desc_AluminaSolution.Desc_AluminaSolution_C': 544,
    '/Game/FactoryGame/Resource/Environment/Berry/Desc_Berry.Desc_Berry_C': 4096,
    '/Game/FactoryGame/Resource/Environment/Nut/Desc_Nut.Desc_Nut_C': 1024,

    "/Game/FactoryGame/Resource/Parts/QuantumEnergy/Desc_QuantumEnergy.Desc_QuantumEnergy_C": 64,
};
const considerRecipes = [
    'Recipe_Alternate_EnrichedCoal_C',
    'Recipe_TimeCrystal_C',
    'Recipe_FicsiteIngot_Iron_C',
    'Recipe_DarkEnergy_C',
    'Recipe_Gunpowder_C',
];
const ignoreItems = [
    '/Game/FactoryGame/Resource/Parts/AlienProtein/Desc_AlienProtein.Desc_AlienProtein_C',
    '/Game/FactoryGame/Resource/Parts/AlienDNACapsule/Desc_AlienDNACapsule.Desc_AlienDNACapsule_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_HogParts.Desc_HogParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_HatcherParts.Desc_HatcherParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_StingerParts.Desc_StingerParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_SpitterParts.Desc_SpitterParts_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_CrystalShard.Desc_CrystalShard_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_Crystal.Desc_Crystal_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_Crystal_mk2.Desc_Crystal_mk2_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_Crystal_mk3.Desc_Crystal_mk3_C',

    '/Game/FactoryGame/Resource/Parts/AlienPowerFuel/Desc_AlienPowerFuel.Desc_AlienPowerFuel_C',
    '/Game/FactoryGame/Resource/Parts/NuclearFuelRod/Desc_NuclearFuelRod.Desc_NuclearFuelRod_C',
    '/Game/FactoryGame/Resource/Parts/UraniumCell/Desc_UraniumCell.Desc_UraniumCell_C',

    '/Game/FactoryGame/Resource/Parts/BioFuel/Desc_PackagedBiofuel.Desc_PackagedBiofuel_C', // 4806
    '/Game/FactoryGame/Resource/RawResources/Water/Desc_PackagedWater.Desc_PackagedWater_C', // 4800
    '/Game/FactoryGame/Resource/Parts/Alumina/Desc_PackagedAlumina.Desc_PackagedAlumina_C', // 5344
    '/Game/FactoryGame/Resource/Parts/SulfuricAcid/Desc_PackagedSulfuricAcid.Desc_PackagedSulfuricAcid_C', // 6208
    '/Game/FactoryGame/Resource/Parts/PackagedNitrogen/Desc_PackagedNitrogenGas.Desc_PackagedNitrogenGas_C', // 42111
    '/Game/FactoryGame/Resource/Parts/NitricAcid/Desc_PackagedNitricAcid.Desc_PackagedNitricAcid_C', // 42175
];
const ignoreOverrideSinkItems = [
    // not sinkable by default game
    '/Game/FactoryGame/Resource/Parts/AlienProtein/Desc_AlienProtein.Desc_AlienProtein_C',
    '/Game/FactoryGame/Resource/Parts/AlienDNACapsule/Desc_AlienDNACapsule.Desc_AlienDNACapsule_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_HogParts.Desc_HogParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_HatcherParts.Desc_HatcherParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_StingerParts.Desc_StingerParts_C',
    '/Game/FactoryGame/Resource/Parts/AnimalParts/Desc_SpitterParts.Desc_SpitterParts_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_CrystalShard.Desc_CrystalShard_C',
    '/Game/FactoryGame/Resource/Environment/Berry/Desc_Berry.Desc_Berry_C',
    '/Game/FactoryGame/Resource/Environment/Nut/Desc_Nut.Desc_Nut_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_CrystalShard.Desc_CrystalShard_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_Crystal.Desc_Crystal_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_Crystal_mk2.Desc_Crystal_mk2_C',
    '/Game/FactoryGame/Resource/Environment/Crystal/Desc_Crystal_mk3.Desc_Crystal_mk3_C',

    '/Game/FactoryGame/Resource/Parts/AlienPowerFuel/Desc_AlienPowerFuel.Desc_AlienPowerFuel_C',

    // non solid parts/resources
    '/Game/FactoryGame/Resource/Parts/SulfuricAcid/Desc_SulfuricAcid.Desc_SulfuricAcid_C',
    '/Game/FactoryGame/Resource/Parts/NitricAcid/Desc_NitricAcid.Desc_NitricAcid_C',
    '/Game/FactoryGame/Resource/Parts/Alumina/Desc_AluminaSolution.Desc_AluminaSolution_C',
    '/Game/FactoryGame/Resource/Parts/BioFuel/Desc_LiquidBiofuel.Desc_LiquidBiofuel_C',
    '/Game/FactoryGame/Resource/Parts/QuantumEnergy/Desc_QuantumEnergy.Desc_QuantumEnergy_C',
    '/Game/FactoryGame/Resource/RawResources/Water/Desc_Water.Desc_Water_C',
];

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

function saveExportFile(name, content) {
    const filePath = `M:\\Steam\\steamapps\\common\\Satisfactory\\FactoryGame\\Mods\\EquivalentExchange\\Export\\${name}`;
    fs.mkdirSync(path.dirname(filePath), { recursive: true });
    fs.writeFileSync(filePath, content);
}

fetch('http://localhost:8081/getRecipes').then(response => response.json()).then(data => {
    const alternateRecipes = [];
    const recipesLeft = [];
    for (const recipe of data) {
        if (['N/A', 'FICSMAS'].includes(recipe.Category)) continue;

        if (considerRecipes.includes(recipe.ClassName)) {
            recipesLeft.push(recipe);
            continue;
        }
        
        if (recipe.Name.startsWith('Alternate:')) {
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

        if (recipe.ProducedIn.includes('Build_Converter_C')) {
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
            const csvEmcFile = [
                '----,ItemClass,EmcValue',
                ...itemKeys.map(item => {
                    if (ignoreItems.includes(item)) return null;

                    const name = item.split('/').pop().replace(/\.Desc_.*$/, '').replace(/^Desc_/, '');

                    return `${name},"${item}",${Math.floor(emcValues[item])}`;
                }).filter(Boolean),
            ];
            const csvSinkTableFile = [
                '---,ItemClass,Points,OverriddenResourceSinkPoints',
                ...itemKeys.map(item => {
                    if (ignoreOverrideSinkItems.includes(item)) return null;
                    
                    const name = item.split('/').pop().replace(/\.Desc_.*$/, '').replace(/^Desc_/, '');
                    const sinkPointValue = Math.max(0, Math.floor(emcValues[item] / (baseEMCPerSinkPoint / 1)));

                    return `${name},"${item}",${sinkPointValue},${sinkPointValue}`;
                }).filter(Boolean),
            ];

            console.log(`${csvEmcFile.length - 1} items with EMC value, ${csvSinkTableFile.length - 1} sinkable items`);
            saveExportFile('EmcValues.csv', csvEmcFile.join('\n'));
            saveExportFile('SinkTable.csv', csvSinkTableFile.join('\n'));
        }
    }

    calculateRecipes();
    // console.log(recipesLeft.length);
    // N/A
});