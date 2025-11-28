const tiers = [
    // items per minute
    10, // t1 = 60
    30, // t2 = 120
    60, // t3 = 120
    120, // t4 = 270
    270, // t5 = 480
    480, // t6 = 480
    630, // t7 = 780
    780, // t8 = 780
    1200, // t9 = 1200
];

const finalConsumption = {
    0: 10,
};

for ( let i = 1; i < tiers.length; i++ ){
    const required = tiers[i] / tiers[i-1];
    finalConsumption[i] = Math.ceil(required * finalConsumption[i-1] * 1.1);
    if ( finalConsumption[i] > 1000 ) finalConsumption[i] = Math.ceil(finalConsumption[i] / 500) * 500;
    else if ( finalConsumption[i] > 1000 ) finalConsumption[i] = Math.ceil(finalConsumption[i] / 100) * 100;
    else if ( finalConsumption[i] > 10 ) finalConsumption[i] = Math.ceil(finalConsumption[i] / 50) * 50;
}

for ( const key of Object.keys(finalConsumption) ) {
    console.log(parseInt(key)+1, '=', finalConsumption[key] + ' MW,', tiers[key] + '/s');
}
