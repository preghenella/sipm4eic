#pragma once

namespace datasheet {
    
//>>    Keithley 2450 Specifics
//https://download.tek.com/manual/SPEC-2450B_November_2017.pdf
double fVoltageRange    [5] =   { 20*1.e-3,     200*1.e-3,  2.0,        20.0,       200.0       };
double fVoltagePrcnt    [5] =   { 1.e-3,        .15*1.e-3,  .2*1.e-3,   .15*1.e-3,  .15*1.e-3   };
double fVoltageOffSt    [5] =   { 200*1.e-6,    200*1.e-6,  300*1.e-6,  2.4*1.e-3,  24*1.e-3    };
/*
 Temperature coefficient: ± (0.15 × accuracy specification)/°C
 0 °C to 18 °C and 28 °C to 50 °C
 */

double fCurrentRange    [9] =   { 10*1.e-9,     100*1.e-9,  1.e-6,      10.e-6,     100.e-6,    1.e-3,      10.e-3,     100.e-3,    1.,         };
double fCurrentPrcnt    [9] =   { 1.e-3,        .6*1.e-3,   .25*1.e-3,  .25*1.e-3,  .20*1.e-3,  .20*1.e-3,  .20*1.e-3,  .25*1.e-3,  .67*1.e-3   };
double fCurrentOffSt    [9] =   { 100*1.e-12,   150*1.e-12, 400*1.e-12, 1.5*1.e-9,  15*1.e-9,   150*1.e-9,  1.5*1.e-6,  15*1.e-6,   900*1.e-6   };
/*
 Temperature coefficient: ± (0.15 × accuracy specification)/°C
0 °C to 18 °C and 28 °C to 50 °C
*/

//>>    Reference Errors from Ferrara
double  belowI  [6] = {2.e-9, 20.e-9, 200.e-9, 2.e-6, 20.e-6, 200.e-6};
double  percI   [6] = {0.003, 0.002, 0.0015, 0.0015, 0.001, 0.001};
double  fixedI  [6] = {400.e-15, 1.e-12, 10.e-12, 100.e-12, 1.e-9, 10.e-9};
double  percV   [1] = {0.001};
double  fixedV  [1] = {4.e-3};

//>>  Sensor name
std::map<std::string, std::string> sensor = {

    { "SENSL_A" , "MICROFJ-30020-TSV-TR1" } ,
    { "SENSL_B" , "MICROFJ-30035-TSV-TR1" } ,
    { "BCOM"    , "AFBR-S4N33C013" } ,
    { "HAMA1_A" , "S13360-3050VS" } ,
    { "HAMA1_B" , "S13360-3025VS" } ,
    { "HAMA2_A" , "S14160-3050HS" } ,
    { "HAMA2_B" , "S14160-3015PS" }
};

//>>  Number of SPAD cells
std::map<std::string, int> Ncells = {
    { "SENSL_A" , 14410 } ,
    { "SENSL_B" ,  5676 } ,
    { "BCOM"    ,  9815 } ,
    { "HAMA1_A" ,  3584 } ,
    { "HAMA1_B" , 14336 } ,
    { "HAMA2_A" ,  3531 } ,
    { "HAMA2_B" , 39984 }
};
      
//>>  Nominal breakdown voltage
std::map<std::string, double> Vbd = {
    { "SENSL_A" ,  24.5 } ,
    { "SENSL_B" ,  24.5 } ,
    { "BCOM"    ,  26.9 } ,
    { "HAMA1_A" ,  53. } ,
    { "HAMA1_B" ,  53. } ,
    { "HAMA2_A" ,  38. } ,
    { "HAMA2_B" ,  38. }
};

} // namespace datasheet
