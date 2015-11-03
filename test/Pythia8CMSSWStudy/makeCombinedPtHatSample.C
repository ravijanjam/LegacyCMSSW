void makeCombinedPtHatSample(int n, const double * xsec, const char * inputFiles[], const char * outputFile , const char * eventHistName = "events")
{

  // number of pthat bins
  const int MAX_BINS = 100;

  if( n > MAX_BINS ) 
  {
    echo << "This is too many pt-hat samples! Adjust MAX_BINS in makeCombinedPtHatSample.C and try again." 
         << endl;
  }

  // Input files
  TFile * f[MAX_BINS];
  for( int i=0; i<n;i++)
  {
    cout << "Opening input file: " << inputFiles[i] << endl;
    f[i] = new TFile(inputFiles[i]);
  }
  // Target file
  TFile * target = new TFile(outputFile,"RECREATE"); 

  // Source files must contain histograms 1 directory level deep. 
  // all source files must have the same directory structure.
  // 
  // There must be a TH1F called "events" (or eventHistName ) 
  // in each directory of each 
  // file. The integral of this histogram should be the number of events.
  TH1F * events[MAX_BINS];
  double nevt[MAX_BINS]; 

  // loop for top level directories 
  f[0]->cd( );
  TDirectory *current_sourcedir = gDirectory;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {

    // read object from first source file
    f[0]->cd();
    TObject *obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {

      // add directory to the target file
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

      // now enter the subdirectory
      TString path( (char*)strstr( newdir->GetPath(), ":" ) );
      path.Remove( 0, 2 );      
      cout << "Entering directory " << path << endl;
      f[0]->cd( path );

      // count events for each pthat sample in the current subdirectory
      char heventpath[256];
      strcpy( heventpath, path.Data() );
      strcat(heventpath,"/");
      strcat(heventpath,eventHistName);
       
      double nevent[MAX_BINS];

      for( int i=0; i<n; i++) 
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
        f[0]->cd( path );
        TObject *obj2 = key2->ReadObj();

        if ( obj2->IsA()->InheritsFrom( TH1::Class() ) ) {
         // descendant of TH1 -> merge it

         // skip the events histogram
         if( ! strcmp(obj2->GetName(),eventHistName) )
           continue;
         
         char hpath[256]; 
         strcpy(hpath,path.Data()); 
         strcat(hpath,"/"); 
         strcat(hpath,obj2->GetName());

         TH1 *hin[MAX_BINS];
         for(int i=0; i<n; i++)
         {
           hin[i] = (TH1*) f[i]->Get(hpath);
           hin[i]->Sumw2();
           hin[i]->Scale(xsec[i]/nevent[i]);
         }
 
         for( int i=1; i<n; i++)
           hin[0]->Add(hin[i]);
         
         target->cd( path );
         hin[0]->Write(); 

        }
      }
    }
  } 
  
}
