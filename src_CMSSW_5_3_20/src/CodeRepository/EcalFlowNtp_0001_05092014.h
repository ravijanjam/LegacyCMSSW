#ifndef EcalFlowNtp_H
#define EcalFlowNtp_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include <DataFormats/VertexReco/interface/Vertex.h>
// Geometry
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/EcalEndcapTopology.h"
#include "Geometry/CaloTopology/interface/EcalBarrelTopology.h"
#include "RecoEcal/EgammaCoreTools/interface/PositionCalc.h"

// ES stuff
#include "DataFormats/EgammaReco/interface/PreshowerCluster.h"
#include "RecoEcal/EgammaClusterAlgos/interface/PreshowerClusterAlgo.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

// ROOT stuff
#include "TH1.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"
#include <TRandom1.h>

#include <map>
#include <set>
class SimTrack;
class SimVertex;

#define bufferDepth 3 
#define maximumNumberAcceptedCluster 1000
typedef std::map<DetId, EcalRecHit> RecHitsMap;

// Less than operator for sorting EcalRecHits according to energy.
class ecalRecHitLess : public std::binary_function<EcalRecHit, EcalRecHit, bool> 
{
public:
  bool operator()(EcalRecHit x, EcalRecHit y) 
  { 
    return (x.energy() > y.energy()); 
  }
};




class TFile;
class TTree;


//class DQMStore;
//class MonitorElement;

#define NCRYMAX 35000
#define NCLUMAX 10000
#define NPI0MAX 0
#define NCONVMAX 500
#define NMAXTRACKS 10000

// Testing - Ravi - BEGIN
class myClass : public edm::EDAnalyzer {

	void beginJob();
//	void analyze(const edm::Event& e, const edm::EventSetup& c);
};


// Testing - Ravi - END

class EcalFlowNtp : public edm::EDAnalyzer {

public:

  explicit EcalFlowNtp( const edm::ParameterSet& );
  ~EcalFlowNtp();

protected:
   
  void beginJob();

  void analyze(const edm::Event& e, const edm::EventSetup& c) ;

  /*void beginLuminosityBlock(const edm::LuminosityBlock& lumiSeg, 
                            const edm::EventSetup& context) ;

  void endLuminosityBlock(const edm::LuminosityBlock& lumiSeg, 
                          const edm::EventSetup& c);*/

  void endJob();
  void convxtalid(int & , int &);
  void initHistos(const edm::Service<TFileService> & fs);
  bool TrackQualityCuts(const reco::Track & track, const reco::Vertex & vertexCollectionSelected);
  static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );
  int diff_neta_s(int,int);
  int diff_nphi_s(int,int);
 
private:
  // Added by Monika Sharma for reading tracks                                                                              

  bool occByCentrality_;
  bool diHadronCorrelations_;
  bool pi0HadronCorrelations_;
  bool etaHadronCorrelations_;
  bool doEffCorrections_;
  bool rotatedBackground_;
  bool useClusterCrystalLimitLowPtOnly_;
  bool avoidIeta85_;

  double cutDzErrMax_;
  double cutDxyErrMax_;
  double cutPtErrMax_;
  double leadingTrackPtMin_;
  double leadingTrackPtMax_;
  double leadingPhotonPtMin_;
  double leadingPhotonPtMax_;
  double etaCut_;
  double ptCut_;
  double etHFtowerSumPlus_;
  double etHFtowerSumMinus_;
  double etHFtowerSumTotal_;
  double swissThreshold_;
  double timeThreshold_;
  double clusterCrystalLimitLowPtOnly_;
  int tHighPurityTracks_;
  int AllTracks_;


  TH1F *tEvents_;
  TH1I *tHit_;
  TH1F *tPt_;
  TH1F *EventsLeadTrack_;
  TH1F *tPhi_;
  TH1F *tEta_;
  TH1F *tPt_trg_;
  TH1F *tPhi_trg_;
  TH1F *tEta_trg_;
  TH1F *tVertexX_;
  TH1F *tVertexY_;
  TH1F *tVertexZ_;
  TH1F *tHPTracks_;
  TH1F *tAllTracks_;
  TH1D *_DeltazVertex;
  TH1F *tDz_;
  TH1F *tDxy_;
  TH1F *tPtQA_;
  TH2F *tEtvsHiTracksPlus_;
  TH2F *tEtvsHiTracksMinus_;
  TH2F *tEtvsHPHiTracksPlus_;
  TH2F *tEtvsHPHiTracksMinus_;
  TH2F *effhisto;
  TH2D *effhisto_pi0;

  TH1D *pi0MassHistSameEvent;
  TH1D *pi0MassHistMixed;
  TH1D *pi0EtaTrueHist;
  TH1D *pi0PhiTrueHist;
  TH1D *pi0PtTrueHist;
  TH1D *ShowerShapeCut;
  TH1D *PhotonClusterPt;
  TH1D *EventsLeadPhotonPt;
  TH1D *PFPhotonPt;
  TH1D *Timing;
  TH1D *TimingInClustering;
  TH1D *SwissCrossCut;
  TH3F *_pi0Spectrum;

  std::map<std::string,TH1D*> pi0MeanPt;
  std::map<std::string,TH1D*> pi0MassHistSameEventvsPt;
  std::map<std::string,TH1D*> pi0MassHistMixedEventvsPt;
  std::map<std::string,TH2D*> hSignal;
  std::map<std::string,TH2D*> hBackground;
  TH2D *hSignal1;
  TH2D *hBackground1;

  int aHit[NMAXTRACKS];
  double aPt[NMAXTRACKS];
  double aPhi[NMAXTRACKS];
  double aEta[NMAXTRACKS];

  std::string srcTracks_; 
  edm::InputTag vertexSrc_;
  edm::InputTag srcVertex_;
  edm::InputTag srcTowers_;
  edm::InputTag particleFlow_;
  
  /////////////// added for pi0 invariant mass //////
  int nMassBins;
  int bins;
  int bins1;
  float clustEnrCut;
  float clustS49Cut;
  float clustPtCut;
  float clustS1Cut;
  float clustS25Cut;
  
  int countMixedClusterExamined;
  float ppConversionCutRadius;
  float minimumSeparation;
  float minimumSeparationSquared;
  bool usePtDependentSeparation;
  char countsPerBin[100];
  int binWidth;
  int countSameEvents;
  int countSeparationPtCut;
  double aSeparationCoefficient;
  double bSeparationCoefficient;
  double rescaleSeparationFactor;
  double vertexZMax_;
  double cutMultMin_;
  double cutMultMax_;
  double cutByLeadingTrackPt_;
  double cutByLeadingPhotonPt_;
  double cutMinTrack_;
  double lowpi0PtCut_;
  double highpi0PtCut_;
  bool useFixedOpeningAngleCut;
  bool failOpeningAngleCut;
  float maximumPi0MassForHistogram;
  int nBinsOneD;
  //  float lowPtCut;
  // float highPtCut;
  float aOpenAngleCutParameter;
  float bOpenAngleCutParameter;
  float lowEtaLimit;
  float highEtaLimit;
  int maximumClustersInSingleEvent;
  int countAcceptedCluster[bufferDepth];
  Long64_t eventNumberCluster[bufferDepth];
  int currentAcceptedClusterNumber;
  int totalEventsWithClusters;
  int currentBufferEventIndexCluster;
  int minimumClustersPerEvent;
  float clusterEnergyAsymmetryCut;
  bool useClusterEnergyAsymmetryCut;
  std::vector<double> ptBins_;
  std::vector<double> NptBins_;
  std::vector<double> etaBins_;
  std::vector<double> occBins_;

  //////////// Added for timing cuts ///////////////////

  bool doOneDimensionTimeCut_;
  double oneDimensionTimeCut_;
  bool doTwoDimensionTimeCut_;
  double twoDimensionTimeCutFactor_;
  bool doWeightedTimeCut_;

  //========================Here's the struct===========
  struct acceptedClusterInformation {  
    Long64_t thisEvent;
    unsigned int kCluster;
    unsigned int nCryClu; 
    Float_t clustEta;
    Float_t clustPhi;
    Float_t clustPt;
    Float_t clustEnr;
    Float_t clustS49;
    Float_t clustS25;
    Float_t closestNeighbor;
    Float_t hfAllAngle;
  };
  acceptedClusterInformation acceptedClusterInformationArray[bufferDepth][maximumNumberAcceptedCluster];

  ///////////////////////////////////////////////////

  ///////////////////////////////////////////////////

  ////// for pi0-hadron correlations ////////////////
  std::vector<std::vector<TVector3>> pVectVect_trg;
  std::vector<std::vector<TVector3>> pVectVect_ass;
  std::vector<double> zvtxVect;

  double etaMin_trg_;
  double etaMax_trg_;
  double etaMin_ass_;
  double etaMax_ass_;
  double ptMin_trg_;
  double ptMax_trg_;
  double ptMin_ass_;
  double ptMax_ass_;
  double _pi;
  int bkgFactor_;

  //////////////////////////////////////////////////


   // Method for iterative printing of decay chains 
   bool printChildren(const SimTrack* p, 
        std::map<const SimTrack*, std::set<const SimTrack*> > const& ptokids,
        std::map<const SimTrack*, const SimVertex*> const& ptovtx,
        int level, bool save, int motherGenIndex);
   
   // Remove unneeded SimTracks from tables
    bool pruneKids(const SimTrack* p,
         std::map<const SimTrack*, std::set<const SimTrack*> > & decays,
         std::map<const SimTrack*, const SimTrack*> & parent,
         std::map<const SimTrack*, const SimVertex*> & vertex,
         int level);

    // Energy recalibration function                                                                                       
    float crystalCorrectionFunction(int indexCrystal, float ecalEnergy);
    float efficiencyCorrections(double pTtrig, double eta_trg, int tHighPurityTracks_);
    bool useClusterCrystalLimit_;
    int lowCrystalLimit_;
    int highCrystalLimit_;

    // energy recalibration                                                                                                  
    bool doEnergyRecalibration_;

  //DQMStore*   dbe_;  
  int eventCounter_;      
  PositionCalc posCalculator_ ;                        

  /// Distribution of number of crystals in cluster   
  MonitorElement * hnxtsigEB_;
  MonitorElement * hnxtbkgEB_;

  /// Distribution of rechits in time (pi0)   
  MonitorElement * htimeEB_;
  MonitorElement * htimeEE_;

  /// Pi0 invariant mass in EB
  MonitorElement * hMinvPi0EB_;

  /// Pi0 invariant mass in EE
  MonitorElement * hMinvPi0EE_;

  /// Eta invariant mass in EB
  MonitorElement * hMinvEtaEB_;

  /// Eta invariant mass in EE
  MonitorElement * hMinvEtaEE_;


  /// object to monitor
  edm::InputTag productMonitoredEBpi0_;
  edm::InputTag productMonitoredEBeta_;

 /// object to monitor
  edm::InputTag productMonitoredEEpi0_;
  edm::InputTag productMonitoredEEeta_;

 /// object to monitor
  edm::InputTag productMonitoredESpi0_;
  edm::InputTag productMonitoredESeta_;

 /// conversions
  edm::InputTag productConversions_;

/// mc truth
  edm::InputTag MCTruthCollection_; 

  int gammaCandEtaSize_;
  int gammaCandPhiSize_;

  double seleXtalMinEnergy_;
  double seleXtalMinEnergyEndCap_;

  double clusSeedThr_;
  int clusEtaSize_;
  int clusPhiSize_;

  double clusSeedThrEndCap_;

      //// for pi0->gg barrel 
      double selePtGamma_;
      double selePtPi0_;
      double seleMinvMaxPi0_;
      double seleMinvMinPi0_;
      double seleS4S9Gamma_;
      double selePi0BeltDR_;
      double selePi0BeltDeta_;
      double selePi0Iso_;
      double ptMinForIsolation_; 

      ///for pi0->gg endcap
      double selePtGammaEndCap_;
      double selePtPi0EndCap_;
      double seleMinvMaxPi0EndCap_;
      double seleMinvMinPi0EndCap_;
      double seleS4S9GammaEndCap_;
      double selePi0IsoEndCap_;
      double selePi0BeltDREndCap_;
      double selePi0BeltDetaEndCap_;
      double ptMinForIsolationEndCap_; 

  double region1_Pi0EndCap_;
  double selePtGammaPi0EndCap_region1_; 
  double selePtPi0EndCap_region1_;
  double region2_Pi0EndCap_;
  double selePtGammaPi0EndCap_region2_; 
  double selePtPi0EndCap_region2_;
  double selePtGammaPi0EndCap_region3_; 
  double selePtPi0EndCap_region3_;

      ///for eta->gg barrel
      double selePtGammaEta_;
      double selePtEta_;
      double seleS4S9GammaEta_; 
      double seleS9S25GammaEta_; 
      double seleMinvMaxEta_; 
      double seleMinvMinEta_; 
      double ptMinForIsolationEta_; 
      double seleEtaIso_; 
      double seleEtaBeltDR_; 
      double seleEtaBeltDeta_; 

      ///for eta->gg endcap
      double selePtGammaEtaEndCap_;
      double seleS4S9GammaEtaEndCap_;
      double seleS9S25GammaEtaEndCap_;
      double selePtEtaEndCap_;
      double seleMinvMaxEtaEndCap_;
      double seleMinvMinEtaEndCap_;
      double ptMinForIsolationEtaEndCap_;
      double seleEtaIsoEndCap_;
      double seleEtaBeltDREndCap_;
      double seleEtaBeltDetaEndCap_;

  double region1_EtaEndCap_;
  double selePtGammaEtaEndCap_region1_; 
  double selePtEtaEndCap_region1_;
  double region2_EtaEndCap_;
  double selePtGammaEtaEndCap_region2_; 
  double selePtEtaEndCap_region2_;
  double selePtGammaEtaEndCap_region3_; 
  double selePtEtaEndCap_region3_;

  ///ES 

  int preshNclust_;
  float preshClustECut;
  double etThresh_;
  double calib_planeX_;
  double calib_planeY_;
  double mip_;
  double gamma_;
  PreshowerClusterAlgo * presh_algo; // algorithm doing the real work
  //PreshowerClusterAlgo::DebugLevel debugL;  

  bool ParameterLogWeighted_;
  double ParameterX0_;
  double ParameterT0_barl_;
  double ParameterT0_endc_;
  double ParameterT0_endcPresh_;
  double ParameterW0_;

  std::vector<EBDetId> detIdEBRecHits; 
  std::vector<EcalRecHit> EBRecHits; 
 
  std::vector<EEDetId> detIdEERecHits; 
  std::vector<EcalRecHit> EERecHits; 



  /// Monitor every prescaleFactor_ events
  unsigned int prescaleFactor_;
  
  /// DQM folder name
  std::string folderName_; 
 
  /// Write to file 
  bool saveToFile_;

  /// which subdet will be monitored
  bool isMonEBpi0_;
  bool isMonEBeta_;
  bool isMonEEpi0_;
  bool isMonEEeta_;
  bool isMonESpi0_;
  bool isMonESeta_;

  /// Masks for EB EE(unused) and ES(unused) 
  bool isMaskEB_;
  bool isMaskEE_;
  bool isMaskES_;

  /// Use Reco Flag from RH
  bool useRecoFlag_;

  /// Files with xtals masked
  std::string maskEBFile_;
  std::string maskEEFile_;
  std::string maskESFile_;


  /// Output file name if required
  std::string fileName_;

  /// Store TTree 


    
  /// trigger counters (to be removed)
  int l1_SingleMuOpen_count;
  int l1_SingleJet6U_count;
  int l1_MinBias_HTT10_count;
  int l1_SingleEG1_count;
  int l1_BscMinBiasOR_count;
  int techTrigger40_count;
  int techTrigger41_count;
  
  // trigger variables
/*
  std::vector<int>* myL1Seeds_;      // trigger bits
  std::vector<std::string>* myL1SeedNames_; // description of the trigger bits
  int nSeeds_;      // total number of trigger bits 
  int nPhysSeeds_;  // number of physics triggers
  int nTechSeeds_;  // number of technical triggers
*/

  static const int MAXL1bits = 200;
  static const int MAXHLTbits = 200;
  int nL1bits;
  int L1bits[MAXL1bits];
  int nL1bitsTech;
  int L1bitsTech[MAXL1bits];

  /// TTree for calibration Pi0 EB
  int             runn;
  int             eventn;
  int             ls;
  float           iso;


  //Centrality
  double centrality_val;
  int centrality_bin;

  double zVertexEvent;
  double zVertexEventSelected;
  int nHITracks;

  double PGAngle;
  double PGsumSin;
  double PGsumCos;
  double NGAngle;
  double NGsumSin;
  double NGsumCos;
  double HFAngle;
  double HFsumSin;
  double HFsumCos;
  double etHFAngle;
  double etHFsumSin;
  double etHFsumCos;

  int maxTracksPerJob;
  int maxClustersPerJob;
  int maxCryPerJob;
  int maxPi0PerJob;

  int eventMaxTracksPerJob;
  int eventMaxClustersPerJob;
  int eventMaxCryPerJob;
  int eventMaxPi0PerJob;

  int runMaxTracksPerJob;
  int runMaxClustersPerJob;
  int runMaxCryPerJob;
  int runMaxPi0PerJob;

  int totalEventsThisJob;
  int totalEventsAllJobs;

  int lowCentralityCut;
  int highCentralityCut;
  int eventsInsideCentralityWindow;
  int eventsOutsideCentralityWindow;
  int hotChannelSeedsDiscarded;
  int hotChannelSatellitesDiscarded;

  Int_t nCry;
  Float_t eCry[NCRYMAX];
  Float_t ptCry[NCRYMAX];
  Float_t timeCry[NCRYMAX];
  Int_t flagCry[NCRYMAX];
  Int_t ietaCry[NCRYMAX];
  Int_t iphiCry[NCRYMAX];
  Int_t iCry[NCRYMAX];
  Int_t iSM[NCRYMAX];
  Float_t etaCry[NCRYMAX];
  Float_t phiCry[NCRYMAX];
 
  Int_t nClu;
  Float_t S1Clu[NCLUMAX];
  Float_t S4Clu[NCLUMAX];
  Float_t S9Clu[NCLUMAX];
  Float_t S25Clu[NCLUMAX];
  Float_t etaClu[NCLUMAX];
  Float_t etaCluUncorrected[NCLUMAX];
  Float_t zClu[NCLUMAX];
  Float_t phiClu[NCLUMAX];
  Float_t ptClu[NCLUMAX];
  Float_t seedTimeClu[NCLUMAX];
  Float_t weightedTimeClu[NCLUMAX];
  Float_t timeClu[NCLUMAX];
  Int_t nCryClu[NCLUMAX];
  Int_t indexCryClu[NCLUMAX][9];

 /* Int_t nPi0;
  Float_t ePi0[NPI0MAX];
  Float_t massPi0[NPI0MAX];
  Float_t ptPi0[NPI0MAX];
  Float_t etaPi0[NPI0MAX];
  Float_t phiPi0[NPI0MAX];
  Float_t isoPi0[NPI0MAX];
  Int_t ietaTTPi0[NPI0MAX];
  Int_t iphiTTPi0[NPI0MAX];
  Int_t indexClu1Pi0[NPI0MAX];
  Int_t indexClu2Pi0[NPI0MAX];*/
  
  // mc truth
  static const int nMaxMC = 350;
  static const int kPhoton = 22;
  static const int kPi0 = 111;
  static const int kElectron = 11;
  bool    isMC;
  Int_t   nMC;
  Int_t   pdgIdMC[nMaxMC];
  Int_t   statusMC[nMaxMC];
  Int_t   motherIDMC[nMaxMC];
  Int_t   motherIndexMC[nMaxMC];
  Float_t ptMC[nMaxMC];
  Float_t eMC[nMaxMC];
  Float_t etaMC[nMaxMC];
  Float_t phiMC[nMaxMC];
  bool    convertedMC[nMaxMC]; 

  // SIM particles (those not already in MC particles list)
  // help to study in-flight decays of Kshort, Lambda etc.
  // These are also useful to study photon conversions 
  static const int nMaxSIM = 350;
  Int_t nSIM;
  Int_t pdgIdSIM[nMaxSIM];
  Int_t statusSIM[nMaxSIM];
  //Int_t motherIDSIM[nMaxSIM]; 
  Int_t motherGenIndexSIM[nMaxSIM]; 
  Float_t ptSIM[nMaxSIM];
  Float_t eSIM[nMaxSIM];
  Float_t etaSIM[nMaxSIM];
  Float_t phiSIM[nMaxSIM];
  Float_t rSIM[nMaxSIM];
  Float_t zSIM[nMaxSIM];

// Reconstructed photon conversions
  Int_t nconvPhot;
  Float_t chi2convPhot[NCONVMAX];
  Float_t ndofconvPhot[NCONVMAX];
  Float_t rconvPhot[NCONVMAX];
  Float_t phiconvPhot[NCONVMAX];
  Float_t zconvPhot[NCONVMAX];
  Int_t ntrkconvPhot[NCONVMAX];
  Float_t eovpconvPhot[NCONVMAX];
  Float_t etaecalconvPhot[NCONVMAX];
  Float_t phiecalconvPhot[NCONVMAX];
  Float_t energyecalconvPhot[NCONVMAX];
  // Extra conversion ID - pairwise
  Int_t algoconvPhot[NCONVMAX];
  Float_t d0convPhot[NCONVMAX];
  Float_t detaecalconvPhot[NCONVMAX];
  Float_t dphiecalconvPhot[NCONVMAX];
  Float_t dphivtxconvPhot[NCONVMAX];
  Float_t pairsepconvPhot[NCONVMAX];
  Float_t pairmassconvPhot[NCONVMAX];
  // Extra conversion ID - trackwise
  Float_t trchi21convPhot[NCONVMAX];
  Float_t trndof1convPhot[NCONVMAX];
  Int_t trqual1convPhot[NCONVMAX];
  Float_t trpt1convPhot[NCONVMAX];
  Float_t trerr1convPhot[NCONVMAX];
  Float_t trchi22convPhot[NCONVMAX];
  Float_t trndof2convPhot[NCONVMAX];
  Int_t trqual2convPhot[NCONVMAX];
  Float_t trpt2convPhot[NCONVMAX];
  Float_t trerr2convPhot[NCONVMAX];
  Float_t phi1convPhot[NCONVMAX];
  Float_t eta1convPhot[NCONVMAX];
  Float_t p1convPhot[NCONVMAX];
  Float_t phi2convPhot[NCONVMAX];
  Float_t eta2convPhot[NCONVMAX];
  Float_t p2convPhot[NCONVMAX];

  int nxt9[2];
  int nevpair;
  
  float xtalE9[2][9];
  int xtalEta9[2][9];
  int xtalPhi9[2][9];
  float xtalEtaf9[2][9];
  float xtalPhif9[2][9];
  
  /// TTree for calibration Eta EB
  int             runn_eta;
  int             eventn_eta;
  float           iso_eta;

  int nxt9_eta[2];
  int nevpair_eta;
  
  float xtalE9_eta[2][25];
  int xtalEta9_eta[2][25];
  int xtalPhi9_eta[2][25];
  float xtalEtaf9_eta[2][25];
  float xtalPhif9_eta[2][25];
  

  /// TTree for calibration Pi0 EE
  int             runn_ee;
  int             eventn_ee;
  float           iso_ee;

  int nxt9_ee[2];
  int nevpair_ee;
  
  float xtalE9_ee[2][9];
  int xtalEta9_ee[2][9];
  int xtalPhi9_ee[2][9];
  float xtalEtaf9_ee[2][9];
  float xtalPhif9_ee[2][9];
  
  /// TTree for calibration Eta EE
  int             runn_ee_eta;
  int             eventn_ee_eta;
  float           iso_ee_eta;

  int nxt9_ee_eta[2];
  int nevpair_ee_eta;
  
  float xtalE9_ee_eta[2][25];
  int xtalEta9_ee_eta[2][25];
  int xtalPhi9_ee_eta[2][25];
  float xtalEtaf9_ee_eta[2][25];
  float xtalPhif9_ee_eta[2][25];
  

  /// file and ttree objects : Pi0 EB
  TFile* m_file;
  TTree* m_tree;
  std::string m_outfilename;

  /// file and ttree objects : Pi0 EE
  TFile* m_file_ee;
  TTree* m_tree_ee;
  std::string m_outfilename_ee;

  /// file and ttree objects : Eta EB
  TFile* m_file_eta;
  TTree* m_tree_eta;
  std::string m_outfilename_eta;

  /// file and ttree objects : Eta EE
  TFile* m_file_ee_eta;
  TTree* m_tree_ee_eta;
  std::string m_outfilename_ee_eta;


};

#endif
