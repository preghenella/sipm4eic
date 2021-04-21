#pragma once

namespace datasheet {

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
