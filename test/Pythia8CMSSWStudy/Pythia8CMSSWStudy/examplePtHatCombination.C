#include "makeCombinedPtHatSample.C"

void examplePtHatCombination() 
{

  int n = 10;

  double xsec[10] = {  7.842e+01,
                       6.506e-01,
                       1.576e-01,
                       2.103e-02,
                       2.901e-03,
                       4.833e-04,
                       9.592e-05,
                       2.243e-05,
                       5.902e-06,
                       2.549e-06 };  

  const char * inputFiles[10] = {
     "AnaQCD_4C_13000TeV_inelastic_0to20_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_20to30_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_30to50_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_50to80_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_80to120_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_120to170_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_170to230_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_230to300_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_300to380_numEvent100000.root",
     "AnaQCD_4C_13000TeV_HardQCDAll_380to10000_numEvent100000.root"
  };

  const char outputFile[256] = "AnaQCD_4C_13000TeV_Combined.root";

  makeCombinedPtHatSample(n,xsec,inputFiles,outputFile);

}
