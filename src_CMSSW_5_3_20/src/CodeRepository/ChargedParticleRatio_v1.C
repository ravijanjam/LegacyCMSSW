// C++ Headers
#include <iostream>

// ROOT Headers
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"


using namespace std; 

//void ChargedParticleRatio_v1(char *rootFile){
void ChargedParticleRatio_v1(){

	TCanvas *c = new TCanvas("c", "", 700, 300);
	TFile *inputFile = new TFile("AnaGENJetR357_5020GeV_Jun19_Z2Combined.root", "r");

	cout << "File opened? " << inputFile->IsOpen();
	TH2D *hist = (TH2D*)inputFile->Get("FlavorMatrixPos");
	TH1D *h = (TH1D*)inputFile->Get("FlavorJetSpectrum");
	c->cd();
	h->Draw();
	
	inputFile->Close();
	cout << "File opened? " << inputFile->IsOpen();


}
