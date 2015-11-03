#include <TFile.h>
#include <iostream>
#include <TDatime.h>
using namespace std;
void xrootdTestVanderbilt(const int iOption=0) {

  TDatime *dateTime = new TDatime;
  int iDate = dateTime->GetDate();
  int iTime = dateTime->GetTime();
  cout << "  Begin Vanderbilt access testing " << iDate << " at " << iTime << endl;

  if(iOption == 0 || iOption == 1) {
    TFile *f = TFile::Open("root://cmsxrootd.fnal.gov//store/test/xrootd/T2_US_Vanderbilt//store/mc/SAM/GenericTTbar/GEN-SIM-RECO/CMSSW_5_3_1_START53_V5-v1/0013/CE4D66EB-5AAE-E111-96D6-003048D37524.root");
    if(f) {
      cout << "\n cmsxrootd.fnal.gov successful access to Vanderbilt" << endl;
      f->ls();
      f->Close();
    }
    else {
      cout << "\n cmsxrootd.fnal.gov unsuccessful access to Vanderbilt" << endl;
    }
    dateTime->Set();     // set to system date/time
    iDate = dateTime->GetDate();
    iTime = dateTime->GetTime();
    cout << "  Completed Vanderbilt access test from FNAL on " << iDate << " at " << iTime << endl;
  } // check on iOption = 0 or iOption = 1
  if(iOption == 0 || iOption == 2) {
    TFile *g = TFile::Open("root://cms-xrd-global.cern.ch//store/test/xrootd/T2_US_Vanderbilt//store/mc/SAM/GenericTTbar/GEN-SIM-RECO/CMSSW_5_3_1_START53_V5-v1/0013/CE4D66EB-5AAE-E111-96D6-003048D37524.root");
    if(g) {
      cout << "\n cms-xrd-global.cern.ch successful access to Vanderbilt" << endl;
      g->ls();
      g->Close();
    }
    else {
      cout << "\n cms-xrd-global.cern.ch unsuccessful access to Vanderbilt" << endl;
    }
    dateTime->Set();     // set to system date/time
    iDate = dateTime->GetDate();
    iTime = dateTime->GetTime();
    cout << "  Completed Vanderbilt access test from CERN on " << iDate << " at " << iTime << endl;
  } // check on iOption = 0 or iOption = 2
  dateTime->Set();     // set to system date/time
  cout << "\n  Completed Vanderbilt access testing " << iDate << " at " << iTime << endl;
return;
}
