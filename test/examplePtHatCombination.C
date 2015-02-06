#include "makeCombinedPtHatSample.C"

void examplePtHatCombination() 
{

  int n = 10;

  double xsec[10] = {   7.842e+01,
                       6.506e-01,
                       1.576e-01,
                       2.103e-02,
                       2.901e-03,
                       4.833e-04,
                       9.592e-05,
                       2.243e-05,
                       5.902e-06,
                       2.549e-06 };  



  char rpthats[10][256] = {"0to20","20to30","30to50","50to80",
                           "80to120","120to170","170to230",
                           "230to300","300to380","380to10000"};


  char * inputFiles[10];
  for( int i=0; i<n;i++)
  {
    if (i == 0 ) inputFiles[i] = Form("AnaQCD_4C_13000TeV_inelastic_%s_numEvent100000.root",rpthats[i]);
    if (i != 0 ) inputFiles[i] = Form("AnaQCD_4C_13000TeV_HardQCDAll_%s_numEvent100000.root",rpthats[i]);
  }

  char outputFile[256] = "AnaQCD_4C_13000TeV_Combined.root";

  makeCombinedPtHatSample(n,xsec,inputFiles,outputFile);

}
