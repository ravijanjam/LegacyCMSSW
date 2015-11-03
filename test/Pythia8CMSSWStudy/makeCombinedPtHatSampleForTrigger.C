{
  gStyle->SetOptStat(0);

  // cross sections from the Pythia simulations:

  // PYTHIA tune 4C, 13000 TeV
  double xsec[10] = { 7.842e+01,
                      6.506e-01,
                      1.576e-01,
                      2.103e-02,
                      2.901e-03,
                      4.833e-04,
                      9.592e-05,
                      2.243e-05,
                      5.902e-06,
                      2.549e-06 };

  

  // need to fix first entry given as 0-inf. Subtract all 
  // other xsecs to get 0-20
  double xsec20toinf = 0.0;
  double totalxsec = xsec[0];
  for( int i=1;i<10;i++) xsec20toinf += xsec[i];
  xsec[0] = xsec[0] - xsec20toinf;

  // open files and get spectra and event counts 
  TFile * f[10];
  char rpthats[10][256] = {"0to20","20to30","30to50","50to80",
                           "80to120","120to170","170to230",
                           "230to300","300to380","380to10000"};

  char dirname[6][256] = {
        "qcdAna",
        "",
        "",
        "",
        "",
        ""
  };

  // retrieve and normalize spectra
  TH1F * leadtrack[10][10];
  TH1F * events[10][10];
  double nevt[10][10]; 
  for( int i=0; i<10;i++)
  {
    if (i == 0 ) f[i] = new TFile(Form("AnaQCD_4C_13000TeV_inelastic_%s_numEvent100000.root",rpthats[i]));
    if (i != 0 ) f[i] = new TFile(Form("AnaQCD_4C_13000TeV_HardQCDAll_%s_numEvent100000.root",rpthats[i]));
    cout << "Processing File " << i << endl; 
    for( int j=0; j<1; j++)
    {
      events[i][j] = (TH1F *) f[i]->Get(Form("%s/events",dirname[j]));
      events[i][j]->SetName(Form("events_%d_%d",i,j));
      nevt[i][j] = events[i][j]->GetBinContent(1);
      leadtrack[i][j] = (TH1F *) f[i]->Get(Form("%s/%s",dirname[j],"lead_track"));
      leadtrack[i][j]->SetName(Form("leadtrack_%d_%d",i,j));
      leadtrack[i][j]->Sumw2();
      for( int bin = 1; bin <= leadtrack[i][j]->GetNbinsX(); bin++)
      {
        leadtrack[i][j]->SetBinContent(bin,
                       leadtrack[i][j]->GetBinContent(bin) 
                       * xsec[i] / nevt[i][j] ); 
        leadtrack[i][j]->SetBinError(bin,
                       leadtrack[i][j]->GetBinError(bin) 
                       * xsec[i] / nevt[i][j]  ); 
      }



    }
  }



  // add spectra

  for( int j=0;j<1;j++)
  {
    for( int i=1;i<10;i++) 
    {
     leadtrack[0][j]->Add(leadtrack[i][j],1.0);
    }
  }

  TH1F * trig = leadtrack[0][0];

 
  TCanvas * c1 = new TCanvas("c1","",600,600);
  c1->cd();
  c1->SetLogy();
  TH1F * hdum = new TH1F("hdum",";lead-track p_{T} [GeV/c];d#sigma/dp_{T} [mb c/GeV]",10,0,100);
  hdum->SetMaximum(50);
  hdum->SetMinimum(1e-10);
  hdum->Draw();

  

  trig->Draw("same");

  TLatex * tex1 = new TLatex( 0.35, 0.75, "PYTHA8 Tune 4C, #sqrt{s} = 13 TeV" );
  tex1->SetNDC();
  tex1->SetTextSize(0.03);
  tex1->Draw();

  cout << "Using instantaneous luminosity of 2294 mb^-1 s^-1" << endl;

  double trigPt[50];
  double trigPtErr[50];
  double trigRate[50];
  Double_t trigRateErr[50];
  for ( int i=0; i<50; i++) 
  {
     trigPt[i] = 2.0 * (double)i;
     trigPtErr[i] = 0.0;
     trigRate[i] = 2294.0 * trig->IntegralAndError( trig->GetXaxis()->FindBin(trigPt[i]),trig->GetXaxis()->FindBin(800),trigRateErr[i],"");
     trigRateErr[i] *= 2294.0;
     cout << "Trigger threshold pT = " << trigPt[i] << " has rate of " << trigRate[i] << " +/- " << trigRateErr[i] << " Hz." << endl;

  } 

  TGraphErrors * grate = new TGraphErrors(50,trigPt,trigRate,trigPtErr,trigRateErr);

  
  TCanvas * c2 = new TCanvas("c2","",600,600);
  c2->cd();
  c2->SetLogy();
  TH1F * hdum2 = new TH1F("hdum2",";trigger threshold p_{T} [GeV/c]; rate (Hz)",10,0,100);
  hdum2->SetMaximum(200000);
  hdum2->SetMinimum(0.01);
  hdum2->Draw();

  grate->Draw("p");
 
  tex1->Draw();
  TLatex * tex2 = new TLatex( 0.35, 0.68, "L = 2294 mb^{-1} s^{-1}" );
  tex2->SetNDC();
  tex2->SetTextSize(0.03);
  tex2->Draw();


  //cout << leadtrack[0][j]->Integral(leadtrack[0][j]->GetXaxis()->FindBin(12.0), leadtrack[0][j]->GetXaxis()->FindBin(500.0)) << endl;

}
