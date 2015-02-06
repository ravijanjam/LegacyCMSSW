{

  // This is an absolute holy mess - but it works
  // 
  // Needs to be cleaned up and made more robust and usable
  //

  // number of pthat bins
  const int K = 10;

  // cross sections from the Pythia simulations:
  // PYTHIA tune 4C, 13000 TeV
  double xsec[K] = { 7.842e+01,
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
  // other xsecs to get 0-X
  // turn this off if you have already done so
  double xsec20toinf = 0.0;
  double totalxsec = xsec[0];
  for( int i=1;i<K;i++) xsec20toinf += xsec[i];
  xsec[0] = xsec[0] - xsec20toinf;

  // generate array of files 
  TFile * f[K];
  char rpthats[K][256] = {"0to20","20to30","30to50","50to80",
                           "80to120","120to170","170to230",
                           "230to300","300to380","380to10000"};
  for( int i=0; i<K;i++)
  {
    if (i == 0 ) f[i] = new TFile(Form("AnaQCD_4C_13000TeV_inelastic_%s_numEvent100000.root",rpthats[i]));
    if (i != 0 ) f[i] = new TFile(Form("AnaQCD_4C_13000TeV_HardQCDAll_%s_numEvent100000.root",rpthats[i]));
  }

  // Target file
  TFile * target = new TFile( "AnaQCD_4C_13000TeV_Combined.root", "RECREATE" ); 

  cout << target->GetPath() << endl;

  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );
 
  cout << path << endl;

  // Source files must contain histograms 1 directory level deep. 
  // all source files must have the same directory structure.
  // 
  // There must be a TH1F called "events" in each directory of each 
  // file. The number of events should be stored in bin 1.
  TH1F * events[K];
  double nevt[K]; 

  f[0]->cd( path );
  TDirectory *current_sourcedir = gDirectory;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;

  // look for top level directories 
  while ( (key = (TKey*)nextkey())) {

    // read object from first source file
    f[0]->cd( path );
    TObject *obj = key->ReadObj();

    if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
      // it's a subdirectory
      cout << "Found subdirectory " << obj->GetName() << endl;

      // add it to the target file
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

      // now enter the subdirectory
      TString subpath( (char*)strstr( newdir->GetPath(), ":" ) );
      subpath.Remove( 0, 2 );      

      f[0]->cd( subpath );

      // count events for each pthat sample in the current subdirectory

      char heventpath[256];
      strcpy( heventpath, subpath.Data() );
      strcat(heventpath,"/events");
      cout << heventpath << endl;
       
      double nevent[K];

      for( int i=0; i<K; i++) 
      {
        TH1F* hevt = (TH1F*) f[i]->Get(heventpath);
        nevent[i] = hevt->Integral();
        cout << nevent[i] << endl;
      }
     
      // Now loop through the keys in the subdirectory 
      // and find the TH1 objects and combine them with correct 
      // cross section weightings

      TDirectory *subcurrent_sourcedir = gDirectory;
      TIter nextkey2( subcurrent_sourcedir->GetListOfKeys() );
      TKey *key2, *oldkey2=0;
      while ( (key2 = (TKey*)nextkey2())) {

        // read object from first source file
        f[0]->cd( subpath );
        TObject *obj2 = key2->ReadObj();

        if ( obj2->IsA()->InheritsFrom( TH1::Class() ) ) {
         // descendant of TH1 -> merge it
         cout << "Merging histogram " << obj2->GetName() << endl;
         if( ! strcmp(obj2->GetName(),"events") )
         {
           cout << "  this is the events histogram, skip it" << endl;  
           continue;
         }
         
         char hpath[256]; 
         strcpy(hpath,subpath.Data()); 
         strcat(hpath,"/"); 
         strcat(hpath,obj2->GetName());
         cout << " histogram path " << hpath << endl; 

         TH1 *hin[K];
         for(int i=0; i<K; i++)
         {
           hin[i] = (TH1*) f[i]->Get(hpath);
           hin[i]->Sumw2();
           hin[i]->Scale(xsec[i]/nevent[i]);
         }
 
         for( int i=1; i<K; i++)
           hin[0]->Add(hin[i]);
         
         target->cd( subpath );
         hin[0]->Write(); 

        }

      }

    }

  } 

  
}
