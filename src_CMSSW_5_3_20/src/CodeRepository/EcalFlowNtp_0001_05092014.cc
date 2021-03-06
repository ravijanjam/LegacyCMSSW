// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

// DQM include files

#include "DQMServices/Core/interface/MonitorElement.h"

// work on collections
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//ES  stuff
#include "Geometry/EcalAlgo/interface/EcalPreshowerGeometry.h"
#include "Geometry/CaloTopology/interface/EcalPreshowerTopology.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EgammaReco/interface/PreshowerClusterFwd.h"

#include "Pi0Analysis/EcalFlowNtp/src/EcalFlowNtp.h"

#include "DataFormats/HeavyIonEvent/interface/Centrality.h"
#include "DataFormats/HeavyIonEvent/interface/CentralityProvider.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
//#include "DataFormats/HeavyIonEvent/interface/EvtPlane.h"
//#include "CondFormats/DataRecord/interface/RPFlatParamsRcd.h"
//#include "CondFormats/RPFlatParams/interface/RPFlatParams.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Math/interface/Point3D.h"

#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
/// EgammaCoreTools
//#include "RecoEcal/EgammaCoreTools/interface/PositionCalc.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalEtaPhiRegion.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalTools.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"



#include "TVector3.h"
#include "assert.h"

/// montecarlo
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h" 
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"


#define TWOPI 6.283185308

#include <iostream>
#include <cmath>
#include <fstream>


using namespace std;
using namespace edm;
using namespace reco;

static int iPrintEtaCorrection = 0;
const int nPrintEtaCorrection = 10;

const double rECal = 1.29;                         // radial distance of ECAL for eta correction	
CentralityProvider * centProvider;

EcalFlowNtp::EcalFlowNtp(const edm::ParameterSet& ps) :
  nMassBins(200),
  clustEnrCut(0.4),
  clustS49Cut(0.74),
  clustPtCut(0.0),
  clustS1Cut(0.2),
  clustS25Cut(0.2),
  countMixedClusterExamined(0),
  ppConversionCutRadius(-0.20),
  minimumSeparation(0.05),
  countSeparationPtCut(0),
  aSeparationCoefficient(35.657),
  bSeparationCoefficient(-4.1995),
  rescaleSeparationFactor(0.70),
  useFixedOpeningAngleCut(true),
  maximumPi0MassForHistogram(0.30),
  nBinsOneD(100),
  //  lowPtCut(1.0),
  // highPtCut(12.0),
  aOpenAngleCutParameter(0.1699),
  bOpenAngleCutParameter(-0.1115),
  lowEtaLimit(-1.5),
  highEtaLimit(1.5),
  totalEventsWithClusters(0),
  minimumClustersPerEvent(0),
  clusterEnergyAsymmetryCut(-1.0),
  etaMin_trg_(-2.5),
  etaMax_trg_(2.5),
  etaMin_ass_(-2.5),
  etaMax_ass_(2.5),
  ptMin_trg_(1.0),
  ptMax_trg_(3.0),
  ptMin_ass_(0.3),
  ptMax_ass_(3.0),
  _pi(3.1415927),
  bkgFactor_(10),
  eventCounter_(0)
{
  
  vertexSrc_ = ps.getUntrackedParameter<edm::InputTag>("vertexSrc",edm::InputTag("offlinePrimaryVertices"));
  srcTracks_ = ps.getUntrackedParameter<std::string>("qualityString",std::string("highPurity"));
  srcTowers_ = ps.getParameter<edm::InputTag>("srcTowers");
  particleFlow_ = ps.getParameter<edm::InputTag>("pfCandidatesTag");
  ptBins_ = ps.getParameter<std::vector<double> >("ptBins");
  NptBins_ = ps.getParameter<std::vector<double> >("NptBins");
  etaBins_ = ps.getParameter<std::vector<double> >("etaBins");
  occBins_ = ps.getParameter<std::vector<double> >("occBins");
  occByCentrality_ = ps.getParameter<bool>("occByCentrality");
  diHadronCorrelations_ = ps.getParameter<bool>("diHadronCorrelations");
  pi0HadronCorrelations_ = ps.getParameter<bool>("pi0HadronCorrelations");
  etaHadronCorrelations_ = ps.getParameter<bool>("etaHadronCorrelations");
  doEffCorrections_ = ps.getParameter<bool>("doEffCorrections");
  lowpi0PtCut_ = ps.getParameter<double>("lowpi0PtCut");
  highpi0PtCut_ = ps.getParameter<double>("highpi0PtCut");
  vertexZMax_ = ps.getParameter<double>("vertexZMax");
  cutMultMin_ = ps.getParameter<double>("cutMultMin");
  cutMultMax_ = ps.getParameter<double>("cutMultMax");
  cutMinTrack_ = ps.getParameter<double>("cutMinTrack");
  cutByLeadingTrackPt_ = ps.getParameter<bool>("cutByLeadingTrackPt");
  leadingTrackPtMin_ = ps.getParameter<double>("leadingTrackPtMin");
  leadingTrackPtMax_ = ps.getParameter<double>("leadingTrackPtMax");
  cutByLeadingPhotonPt_ = ps.getParameter<bool>("cutByLeadingPhotonPt");
  leadingPhotonPtMin_ = ps.getParameter<double>("leadingPhotonPtMin");
  leadingPhotonPtMax_ = ps.getParameter<double>("leadingPhotonPtMax");
  swissThreshold_ = ps.getParameter<double>("swissThreshold");
  timeThreshold_ = ps.getParameter<double>("timeThreshold");
  avoidIeta85_ = ps.getParameter<double>("avoidIeta85");
  
  centProvider = 0;
  currentBufferEventIndexCluster = 0;
  useClusterEnergyAsymmetryCut = clusterEnergyAsymmetryCut > 0.0;
  
  for (int kBuffer=0; kBuffer<bufferDepth; kBuffer++) {
    countAcceptedCluster[kBuffer] = 0;
    eventNumberCluster[kBuffer] = 0;
  }
  
  cutDzErrMax_ = ps.getUntrackedParameter<double>("cutDzErrMax", 3.0);
  cutDxyErrMax_ = ps.getUntrackedParameter<double>("cutDxyErrMax", 3.0);
  cutPtErrMax_ = ps.getUntrackedParameter<double>("cutPtErrMax", 0.1);
  
  minimumSeparationSquared = minimumSeparation*minimumSeparation;
  usePtDependentSeparation=false;
  //maximumNumberAcceptedCluster = 1000;
  
  binWidth = 1000.0*maximumPi0MassForHistogram/nMassBins + 0.001;
  sprintf(countsPerBin, "Counts per %d MeV/c^{2} bin", binWidth);
  
  bins = ptBins_.size()-1;
  bins1 = NptBins_.size()-1;
  cout<<"The pT bin width used are: "<<ptBins_.size()-1<<'\t'<<ptBins_[0]<<'\t'<<ptBins_[bins]<<endl;
  cout<<"pT bins used for the histogram: "<<NptBins_.size()-1<<'\t'<<NptBins_[0]<<'\t'<<NptBins_[bins1]<<endl;  
  
  if(usePtDependentSeparation) {
    cout << "\n Using a pT dependent separation cut parameterization " << aSeparationCoefficient << "/pT - ";
    cout <<  fabs(bSeparationCoefficient)  << "/pT^2";
    cout << ",  with a rescale factor " << rescaleSeparationFactor << endl;
  }
  else {
    cout << "\n Not using a a pT dependent separation cut" << endl;
  }

  doEnergyRecalibration_ =  ps.getParameter<bool> ("doEnergyRecalibration");

  if(doEnergyRecalibration_)
    cout << "\n Cluster energy value will be recalibrated according to number of crystals in the cluster" << endl;
  else
    cout << "\n There is no recalibration of the cluster energy value" << endl;

  useClusterCrystalLimitLowPtOnly_ = ps.getParameter<bool> ("useClusterCrystalLimitLowPtOnly");
  clusterCrystalLimitLowPtOnly_ = ps.getParameter<double> ("clusterCrystalLowPtOnly");

  useClusterCrystalLimit_ = ps.getParameter<bool> ("useClusterCrystalLimit");
  lowCrystalLimit_ = ps.getParameter<int> ("lowCrystalLimit");    // used only if useClusterCrystalLimit is true     
  highCrystalLimit_ = ps.getParameter<int> ("highCrystalLimit");  // used only if useClusterCrystalLimit is true       

  if(useClusterCrystalLimit_) {
    if(useClusterCrystalLimitLowPtOnly_) {
      cout << "\n Only a cluster having crystal number from " << lowCrystalLimit_ << " to " << highCrystalLimit_ << " will be accepted"  << endl;
    }
    else {
      cout << "\n For the pi0 candidate with a pT below " << clusterCrystalLimitLowPtOnly_ ;
      cout << " GeV/c then both clusters must have a crystal number in the range " << lowCrystalLimit_ << " to " << highCrystalLimit_ << endl;
    }
    cout << "\n There is no check on the number of crystals in a cluster" << endl;
  } // check on useClusterCrystalLimit   

  doOneDimensionTimeCut_ = ps.getParameter<bool>("doOneDimensionTimeCut");
  oneDimensionTimeCut_ = ps.getParameter<double>("oneDimensionTimeCut");
  if(doOneDimensionTimeCut_) {
    cout << "\n One dimensional (fixed window) time cut enabled at a value " << oneDimensionTimeCut_;
      }

  doTwoDimensionTimeCut_ = ps.getParameter<bool>("doTwoDimensionTimeCut");
  twoDimensionTimeCutFactor_ = ps.getParameter<double>("twoDimensionTimeCutFactor");
  if(doOneDimensionTimeCut_ && doTwoDimensionTimeCut_) {
    cerr <<"\n\n Input parameters conflict: asking for both a one-dimensional and a two-dimensional energy cut to be true";
    return; }

  if(doTwoDimensionTimeCut_) {
    cout << "\n Two dimensional (Energy vs time) cut enabled at a value " << twoDimensionTimeCutFactor_;
      }

  doWeightedTimeCut_ = ps.getParameter<bool>("doWeightedTimeCut");
  if(doWeightedTimeCut_) {
    if(!doTwoDimensionTimeCut_) {
      cerr <<"\n\n Input parameters conflict: asking for a weighted time cut but not asking for ...";
    }
    else {
      cout << "\n The two dimensional weighted time cut (weighted time - seed time) is enabled";
	}
  }

  // Initilization of track histograms [Monika Sharma]                                                                       
  edm::Service<TFileService> fs;
  initHistos(fs);
  
  //dbe_ = Service<DQMStore>().operator->();
  productMonitoredEBpi0_= ps.getUntrackedParameter<edm::InputTag>("AlCaStreamEBpi0Tag");
  productMonitoredEBeta_= ps.getUntrackedParameter<edm::InputTag>("AlCaStreamEBetaTag");
  
  MCTruthCollection_ = ps.getUntrackedParameter<edm::InputTag>("MCTruthCollection");
  
  clusSeedThr_ = ps.getParameter<double> ("clusSeedThr");
  clusEtaSize_ = ps.getParameter<int> ("clusEtaSize");
  clusPhiSize_ = ps.getParameter<int> ("clusPhiSize");
  if ( clusPhiSize_ % 2 == 0 ||  clusEtaSize_ % 2 == 0)
    edm::LogError("AlCaPi0RecHitsProducerError") << "Size of eta/phi for simple clustering should be odd numbers";
  
  seleXtalMinEnergy_ = ps.getParameter<double>("seleXtalMinEnergy");
  
  //for Pi0 barrel selection
  selePtGamma_ = ps.getParameter<double> ("selePtGamma");  
  selePtPi0_ = ps.getParameter<double> ("selePtPi0");  
  seleMinvMaxPi0_ = ps.getParameter<double> ("seleMinvMaxPi0");  
  seleMinvMinPi0_ = ps.getParameter<double> ("seleMinvMinPi0");  
  seleS4S9Gamma_ = ps.getParameter<double> ("seleS4S9Gamma");  
  selePi0Iso_ = ps.getParameter<double> ("selePi0Iso");  
  ptMinForIsolation_ = ps.getParameter<double> ("ptMinForIsolation");
  selePi0BeltDR_ = ps.getParameter<double> ("selePi0BeltDR");  
  selePi0BeltDeta_ = ps.getParameter<double> ("selePi0BeltDeta");  
  
  posCalculator_ = PositionCalc( ps.getParameter<edm::ParameterSet>("posCalcParameters") );
  
}


EcalFlowNtp::~EcalFlowNtp()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
}

// ------------ method called to for each event  ------------
void
EcalFlowNtp::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  eventCounter_++;
  etHFtowerSumPlus_ = 0;
  etHFtowerSumMinus_ = 0;
  etHFtowerSumTotal_ = 0;
  tHighPurityTracks_ = 0;
  AllTracks_=0;
  currentAcceptedClusterNumber = 0;
  
  int thisJob = iEvent.id().run();
  int thisEvent = iEvent.id().event();
  eventNumberCluster[currentBufferEventIndexCluster] = thisEvent;  
  
  if(currentBufferEventIndexCluster >= bufferDepth) {
    cerr << "\n currentBufferEventIndexCluster " << currentBufferEventIndexCluster << " exceeds buffer depth " << bufferDepth;
    cerr << endl;
    return;
  } // safety check
  
  vector<TVector3> pVect_trg;
  vector<TVector3> pVect_ass;
  
  edm::ESHandle<CaloTopology> theCaloTopology;
  iSetup.get<CaloTopologyRecord>().get(theCaloTopology);
  
  edm::Handle<EcalRecHitCollection> rhEBpi0;
  
  iEvent.getByLabel(productMonitoredEBpi0_, rhEBpi0); 
  
  // Initialize the Position Calc
  const CaloSubdetectorGeometry *geometry_p; 
  //  const CaloSubdetectorGeometry *geometryEE_p;
  const CaloSubdetectorGeometry *geometryES_p;
  const CaloSubdetectorTopology *topology_p;
  //  const CaloSubdetectorTopology *topology_ee;
  
  runn = iEvent.id().run();
  eventn = iEvent.id().event();
  ls = iEvent.luminosityBlock();
  
  if (!centProvider) centProvider = new CentralityProvider(iSetup);
  centProvider->newEvent(iEvent,iSetup);
  
  // Determining occupancy variable by centrality for an event
  double occ = 0;
  if(occByCentrality_) occ = (double) centProvider->getBin();
  
  totalEventsThisJob++;
  totalEventsAllJobs++;
  
  if(centrality_bin < lowCentralityCut || centrality_bin >= highCentralityCut) {
    eventsOutsideCentralityWindow++;
    return;
  }
  eventsInsideCentralityWindow++;      
  
  edm::ESHandle<CaloGeometry> geoHandle;
  iSetup.get<CaloGeometryRecord>().get(geoHandle);     
  //  const CaloGeometry* caloGeom = geoHandle.product();

  geometry_p = geoHandle->getSubdetectorGeometry(DetId::Ecal,EcalBarrel);
  //  geometryEE_p = geoHandle->getSubdetectorGeometry(DetId::Ecal,EcalEndcap);
  geometryES_p = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalPreshower);
  topology_p = theCaloTopology->getSubdetectorTopology(DetId::Ecal,EcalBarrel);
  // topology_ee = theCaloTopology->getSubdetectorTopology(DetId::Ecal,EcalEndcap);
  
  // CaloSubdetectorTopology *topology_es=0;
  // if (geometryES_p) topology_es  = new EcalPreshowerTopology(geoHandle);
  
  vector< vector<EcalRecHit> > RecHitsCluster;
  vector< vector<EcalRecHit> > RecHitsCluster5x5;
  vector<float> s4s9Clus;
  vector<float> s9s25Clus;
  nCry = 0;
  nClu = 0;
  
    
    Handle<std::vector<reco::Vertex> > vertexCollectionSelected;
    iEvent.getByLabel(vertexSrc_,vertexCollectionSelected);
    
    Handle<reco::TrackCollection> tracks;
    iEvent.getByLabel("generalTracks", tracks);
    
    nHITracks = tracks->size();
    if(nHITracks > maxTracksPerJob) {
      maxTracksPerJob = nHITracks;
      eventMaxTracksPerJob = thisEvent;
      runMaxTracksPerJob = thisJob;
    }
    
    std::vector<reco::Vertex> vsorted = *vertexCollectionSelected;
    // sort the vertcies by number of tracks in descending order
    // use chi2 as tiebreaker
    std::sort( vsorted.begin(), vsorted.end(), EcalFlowNtp::vtxSort );
    
    // skip events with no PV, this should not happen
    if( vsorted.size() == 0) return;
    // skip events failing vertex cut
    if( fabs(vsorted[0].z()) > vertexZMax_ ) return;
    tEvents_->Fill(0.5);
    
    zVertexEventSelected = vsorted[0].z();
    tVertexZ_->Fill(zVertexEventSelected);

    ////////////////// Selecting events for a leading track pT 

    int multiplicity =0;
    double leadingTrackPt = 0.0;

    reco::TrackCollection::const_iterator tcol;
    for( tcol = tracks->begin(); tcol != tracks->end() ; ++tcol )
      {
	if( TrackQualityCuts(*tcol, vsorted[0]) )
	  {
	    multiplicity++;
	    if( tcol->pt() > leadingTrackPt ) leadingTrackPt = tcol->pt();
	  }
      }
    EventsLeadTrack_->Fill(leadingTrackPt);
    if (cutByLeadingTrackPt_ )
      {
	if( leadingTrackPt < leadingTrackPtMin_ ) return;
	if( leadingTrackPt > leadingTrackPtMax_ ) return;
      }
    //////////////////////////////////////////////////////////

    ////////////// Particle Flow Objects ////////////////////
    edm::Handle<reco::PFCandidateCollection> pfCandidates;   
    iEvent.getByLabel(particleFlow_, pfCandidates);

    for( unsigned ic1=0; ic1<pfCandidates->size(); ic1++ ) {

      const reco::PFCandidate& cand = (*pfCandidates)[ic1];
     
      if(cand.particleId() != PFCandidate::gamma) continue;
      float _pfpt = cand.pt();
      if(_pfpt < 0.4) continue;
      if(fabs(cand.eta()) > 1.5) continue;

      PFPhotonPt->Fill(_pfpt);
      }

    /////////////////////////////////////////////////////////

    
    for( const auto & vi :  vsorted )
      {
	//vi.tracksSize() would fetch the total track size
	tVertexX_->Fill(vi.x());
	tVertexY_->Fill(vi.y());
	//	tVertexZ_->Fill(vi.z());
      }
    
    // comparisons between first and additional primary vertices
    /*    for(unsigned int i =1; i<vsorted.size(); i++)
      {
	double dz = fabs( vsorted[i].z() - vsorted[0].z() );
	double dx = fabs( vsorted[i].x() - vsorted[0].x() );
	double dy = fabs( vsorted[i].y() - vsorted[0].y() );
	double dxy  = sqrt ( dx*dx + dy*dy );
	}*/
    
    // use vertex with most tracks as primary vertex
    // determine position and error
    math::XYZPoint vtxPoint(0.0,0.0,0.0);
    double vzErr =0.0, vxErr=0.0, vyErr=0.0;
    if( vsorted.size() != 0 )
      {
	vtxPoint=vsorted.begin()->position();
	vzErr=vsorted.begin()->zError();
	vxErr=vsorted.begin()->xError();
	vyErr=vsorted.begin()->yError();
      }
    
    for( const auto & track : *tracks ) //de-referencing the pointer "tracks" to track and auto will automatically know the type of tracks. This is a new way of looping over the tracks. It is guaranteed to run over all the tracks. 
      {
	double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
	dxy = track.dxy(vtxPoint);
	dz = track.dz(vtxPoint);
	dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
	dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);
	
	if( !TrackQualityCuts(track, vsorted[0])) continue;
	
	tHighPurityTracks_++;	
	
	tDz_->Fill(dz/dzsigma);
	tDxy_->Fill(dxy/dxysigma);
	tPtQA_->Fill(track.ptError()/track.pt());
	
	tHit_->Fill(track.numberOfValidHits());
	
      }
    
    tAllTracks_->Fill(nHITracks);  
    
    if( !(tHighPurityTracks_ >= cutMultMin_ && tHighPurityTracks_ < cutMultMax_)) return;
    tHPTracks_->Fill(tHighPurityTracks_);
    
    for( const auto & track : *tracks)
      {
	if( !TrackQualityCuts(track, vsorted[0])) continue;
	
	TVector3 pvector;
	pvector.SetPtEtaPhi(track.pt(),track.eta(),track.phi());
	
	if(track.eta()<=etaMax_ass_ && track.eta()>=etaMin_ass_
	   && track.pt()<=ptMax_ass_ && track.pt()>=ptMin_ass_)
	  {
	    pVect_ass.push_back(pvector);
	    tPt_->Fill(track.pt());
	    tEta_->Fill(track.eta());
	    tPhi_->Fill(track.phi());
	  }
	
	if(diHadronCorrelations_) {
	  TVector3 pvector1;
	  pvector1.SetPtEtaPhi(track.pt(),track.eta(),track.phi());
	  
	  if(track.eta()<=etaMax_trg_ && track.eta()>=etaMin_trg_
	     && track.pt()<=ptMax_trg_ && track.pt()>=ptMin_trg_)
	    {
	      pVect_trg.push_back(pvector);
	      tPt_trg_->Fill(track.pt());
	      tEta_trg_->Fill(track.eta());
	      tPhi_trg_->Fill(track.phi());
	    }
	}
	
      }
    
    
    ////////////////// New addition for Et of HF ////////////
    edm::Handle<CaloTowerCollection> towers;
    iEvent.getByLabel(srcTowers_,towers);
    
    for( size_t i = 0; i<towers->size(); ++ i){
      const CaloTower & tower = (*towers)[i];
      double eta = tower.eta();
      bool isHF = tower.ietaAbs() > 29;
      
      if(isHF && eta > 0){
	etHFtowerSumPlus_ += tower.pt();
      }
      if(isHF && eta < 0){
	etHFtowerSumMinus_ += tower.pt();
      }
    }
    
    etHFtowerSumTotal_ = etHFtowerSumPlus_ + etHFtowerSumMinus_;
    tEtvsHiTracksPlus_->Fill(nHITracks, etHFtowerSumPlus_);
    tEtvsHiTracksMinus_->Fill(nHITracks, etHFtowerSumMinus_);
    tEtvsHPHiTracksPlus_->Fill(tHighPurityTracks_, etHFtowerSumPlus_);
    tEtvsHPHiTracksMinus_->Fill(tHighPurityTracks_, etHFtowerSumMinus_);
    /////////////////////////////////////////////////////////

    
    //Read collection
    
    std::vector<EcalRecHit> seeds;
    seeds.clear();
    
    vector<EBDetId> usedXtals;
    usedXtals.clear();
    
    detIdEBRecHits.clear(); // EBDetId
    EBRecHits.clear();  //EcalRecHit
    
    const EcalRecHitCollection *hitCollection_p = rhEBpi0.product();
    float etot =0;
    EcalRecHitCollection::const_iterator itb;

    if (rhEBpi0.isValid() && (rhEBpi0->size() > 0)){
      
      //      double rhEt;
      for(itb=rhEBpi0->begin(); itb!=rhEBpi0->end(); ++itb){
	
	////////////// Applying the swiss-cross and timing cuts/////
	//      const GlobalPoint &position = caloGeom->getPosition(itb->id());
	//      rhEt = itb->energy()/cosh(position.eta()); // Not using at the moment 
	double  swissCrx = EcalTools::swissCross  (itb->id(), *hitCollection_p, 0., avoidIeta85_);
	SwissCrossCut->Fill(swissCrx);
	Timing->Fill(abs(itb->time()));
	
	//	if(    (swissCrx > swissThreshold_)     ||     ( abs(itb->time()) > timeThreshold_) ) continue;
	//	if( swissCrx > swissThreshold_ ) continue;
	//	if( abs(itb->time()) > timeThreshold_ ) continue;
	
	////////////////////////////////////////////////////////////
	
	EBDetId id(itb->id());
	double energy = itb->energy();
	if( energy < seleXtalMinEnergy_) continue; 
	
	EBDetId det = itb->id();
	
	detIdEBRecHits.push_back(det);
	EBRecHits.push_back(*itb);	  
	if ( energy > clusSeedThr_ ) seeds.push_back(*itb);
	
	//htimeEB_->Fill(itb->time());	  
	etot+= itb->energy();	 
	
      } // Eb rechits
      
      iso = 0;
      
      // Make own simple clusters (3x3, 5x5 or clusPhiSize_ x clusEtaSize_)
      sort(seeds.begin(), seeds.end(), ecalRecHitLess());
      
      //int seedcount(0);
      
      for (std::vector<EcalRecHit>::iterator itseed=seeds.begin(); itseed!=seeds.end(); itseed++) {
	EBDetId seed_id = itseed->id();
	std::vector<EBDetId>::const_iterator usedIds;
	
	int nCryinclu(0);
	bool seedAlreadyUsed=false;
	for(usedIds=usedXtals.begin(); usedIds!=usedXtals.end(); usedIds++){
	  if(*usedIds==seed_id){
	    seedAlreadyUsed=true;
	    //cout<< " Seed with energy "<<itseed->energy()<<" was used !"<<endl;
	    break; 
	  }
	}
	if(nClu>=NCLUMAX) continue;
	if(nCry>=NCRYMAX) continue;

	double seedTime = itseed->time();
	TimingInClustering->Fill(seedTime);
	//	if( seedTime > timeThreshold_ ) continue;
	double weightedTime = 0.0;

	if(seedAlreadyUsed)continue;
	std::vector<DetId> clus_v = topology_p->getWindow(seed_id,clusEtaSize_,clusPhiSize_);       
	std::vector<std::pair<DetId,float> > clus_used;
	//Reject the seed if not able to build the cluster around it correctly
	//if(clus_v.size() < clusEtaSize_*clusPhiSize_){cout<<" Not enough RecHits "<<endl; continue;}
	vector<EcalRecHit> RecHitsInWindow;
	vector<EcalRecHit> RecHitsInWindow5x5;
	
	double simple_energy = 0;
	
	for (std::vector<DetId >::iterator det=clus_v.begin(); det!=clus_v.end(); det++) {
	  EBDetId EBdet = *det;
	  //      cout<<" det "<< EBdet<<" ieta "<<EBdet.ieta()<<" iphi "<<EBdet.iphi()<<endl;
	  bool  HitAlreadyUsed=false;
	  for(usedIds=usedXtals.begin(); usedIds!=usedXtals.end(); usedIds++){
	    if(*usedIds==*det){
	      HitAlreadyUsed=true;
	      break;
	    }
	  }
	  if(HitAlreadyUsed)continue;
	  
	  std::vector<EBDetId>::iterator itdet = find( detIdEBRecHits.begin(),detIdEBRecHits.end(),EBdet);
	  if(itdet == detIdEBRecHits.end()) continue; 
	  
	  int nn = int(itdet - detIdEBRecHits.begin());
	  usedXtals.push_back(*det);
	  RecHitsInWindow.push_back(EBRecHits[nn]);
	  clus_used.push_back(std::make_pair(*det,1));
	  simple_energy = simple_energy + EBRecHits[nn].energy();
	  
	}
	
	if(simple_energy <= 0) continue;
	
	math::XYZPoint clus_pos = posCalculator_.Calculate_Location(clus_used,hitCollection_p,geometry_p,geometryES_p);
	
	float theta_s = 2. * atan(exp(-clus_pos.eta()));
	
	// eta correction according to Z
	double etaForZEqualsZero = clus_pos.eta();  // initial eta value of cluster without correcting for event Z-vertex;
	double zCorrectedEta = etaForZEqualsZero; // placeholder for correction result
	double zAtECal = 0.0;
	double thetaForZEqualsZero = -1000.0;
	if(etaForZEqualsZero != 0.0) {
	  thetaForZEqualsZero = 2.0*atan(exp(-etaForZEqualsZero));
	  zAtECal = rECal/tan(thetaForZEqualsZero);
	} // check for non-zero value of uncorrected eta
	
	//
	// Correction for zVertex using the zVertexEventSelected value
	//
	double zCorrected = zAtECal - 0.01*zVertexEventSelected;  // convert to meters
	double thetaCorrected = -1000.0;
	if(zCorrected != 0.0) {
	  
	  thetaCorrected = atan(rECal/zCorrected);
	  if(thetaCorrected < 0.0)
	    thetaCorrected += TMath::Pi();  // convert to 0 to pi range for theta
	  
	  theta_s = thetaCorrected;  // corrected value of theta
	  
	  zCorrectedEta = -log(tan(thetaCorrected/2.0));
	}
	else {
	  zCorrectedEta = 0.0;
	} // check for non-zero value of Z baseline
	
	if(iPrintEtaCorrection < nPrintEtaCorrection) {
	  cout << "\n First pass: zVertexEventSelected = " << 0.01*zVertexEventSelected;  // convert to meters
	  cout << ", zAtECal = " << zAtECal;
	  cout << ", zCorrected = " << zCorrected;
	  cout << ", thetaForZEqualsZero = " << thetaForZEqualsZero;
	  cout << ", thetaCorrected = " << thetaCorrected;
	  cout << ", zCorrectedEta = " << zCorrectedEta;
	  cout << ", uncorrected Eta = " << etaForZEqualsZero;
	  
	  iPrintEtaCorrection++;
	  if(iPrintEtaCorrection >= nPrintEtaCorrection)
	    cout << endl;
	  
	}
	
	float et_s = simple_energy * sin(theta_s);
	
	float s4s9_tmp[4];
	for(int i=0;i<4;i++)s4s9_tmp[i]= 0;
	
	int seed_ieta = seed_id.ieta();
	int seed_iphi = seed_id.iphi();
	
	convxtalid( seed_iphi,seed_ieta);
	
	// Getting rid of hot channels 
	// for ECAL barrel this method converts the iphi index range to 0 -> 359, and the ieta range to -85 -> -1 (negative eta) or 0 -> 84 (positive eta)      
	if((seed_ieta == -75 && seed_iphi == 199) ||
	   (seed_ieta ==  67 && seed_iphi == 74)) {
	  hotChannelSeedsDiscarded++;
	  continue;
	}
	
	float e3x3 = 0; 
	float e5x5 = 0;
	double e4_e1 = 0;
	//	double Swiss_cross;
	
	for(unsigned int j=0; j<RecHitsInWindow.size();j++){
	  EBDetId det = (EBDetId)RecHitsInWindow[j].id(); 
	  
	  int ieta = det.ieta();
	  int iphi = det.iphi();
	  
	  convxtalid(iphi,ieta);
	  
	  //Getting rid of the hot channel	
	  if((ieta == -75 && iphi == 199) ||
	     (ieta ==  67 && iphi == 74)) {
	    hotChannelSatellitesDiscarded++;
	    continue;
	  }
	  
	  float en = RecHitsInWindow[j].energy(); 
	  
	  int dx = diff_neta_s(seed_ieta,ieta);
	  int dy = diff_nphi_s(seed_iphi,iphi);
	  
	  double eRight = 0;
	  double eTop = 0;
	  double eLeft = 0;
	  double eBottom = 0; 	
	  /////////// adding swiss-cross cut ////
	  
	  if(dx == 1) eRight =  en;
	  if(dy == 1) eTop =  en;
	  if(dy == -1) eLeft =  en;
	  if(dx == -1) eBottom =  en;
	  
	  e4_e1 += (eRight + eTop + eLeft + eBottom);
	  //	  Swiss_cross = 1 - e4_e1;
	  
	  //	if(Swiss_cross > 0.87) continue;	//Not applying the swiss-cross cut
	  ///////////////////////////////////////
	  if(abs(dx)<=1 && abs(dy)<=1) {
	    e3x3 += en; 
	    if(dx <= 0 && dy <=0) s4s9_tmp[0] += en; 
	    if(dx >= 0 && dy <=0) s4s9_tmp[1] += en; 
	    if(dx <= 0 && dy >=0) s4s9_tmp[2] += en; 
	    if(dx >= 0 && dy >=0) s4s9_tmp[3] += en; 
	    if(nCry>=NCRYMAX) continue;
	    eCry[nCry] = RecHitsInWindow[j].energy();
	    ptCry[nCry] =  RecHitsInWindow[j].energy() * sin(2. * atan(exp(-geometry_p->getGeometry(det)->getPosition().eta())));
	    timeCry[nCry] = RecHitsInWindow[j].time();
	    flagCry[nCry] = RecHitsInWindow[j].recoFlag();
	    ietaCry[nCry] = ieta;
	    iphiCry[nCry] = iphi;
	    weightedTime += eCry[nCry]*timeCry[nCry];
	    iCry[nCry] = det.ic();
	    iSM[nCry] = det.ism();
	    etaCry[nCry] = geometry_p->getGeometry(det)->getPosition().eta();
	    phiCry[nCry] = geometry_p->getGeometry(det)->getPosition().phi();
	    indexCryClu[nClu][nCryinclu] = nCry;
	    if(dx == 0 && dy == 0)  {
	      S1Clu[nClu] = RecHitsInWindow[j].energy();
	      timeClu[nClu] = RecHitsInWindow[j].time();
	    }
	    nCryinclu++;
	    nCry++;      
	  }//
	}
	
	if(e3x3 <= 0) continue;
	
	float s4s9_max = *max_element( s4s9_tmp,s4s9_tmp+4)/e3x3; 
	
	//calculate e5x5
	std::vector<DetId> clus_v5x5 = topology_p->getWindow(seed_id,5,5); 
	for( std::vector<DetId>::const_iterator idItr = clus_v5x5.begin(); idItr != clus_v5x5.end(); idItr++){
	  EBDetId det = *idItr;
	  
	  //inside collections
	  std::vector<EBDetId>::iterator itdet = find( detIdEBRecHits.begin(),detIdEBRecHits.end(),det);
	  if(itdet == detIdEBRecHits.end()) continue; 
	  
	  int nn = int(itdet - detIdEBRecHits.begin());
	  
	  RecHitsInWindow5x5.push_back(EBRecHits[nn]);
	  e5x5 += EBRecHits[nn].energy();
	}
	
	if(e5x5 <= 0) continue;
	
	RecHitsCluster.push_back(RecHitsInWindow);
	RecHitsCluster5x5.push_back(RecHitsInWindow5x5);
	
	S9Clu[nClu] = e3x3;
	ptClu[nClu] =  et_s;
	// etaClu[nClu] = clus_pos.eta();
	etaClu[nClu] = zCorrectedEta;
	phiClu[nClu] = clus_pos.phi();
	S4Clu[nClu] =  s4s9_max * simple_energy;
	S25Clu[nClu] = e5x5;
	nCryClu[nClu] = nCryinclu;
	seedTimeClu[nClu] = seedTime;
	weightedTimeClu[nClu] = weightedTime/e3x3;	

	if(doEnergyRecalibration_) {
	  
	  if(nCryinclu < 1 || nCryinclu > 9) {
	    cerr << "\n\n Programming error with nCryinclu = " << nCryinclu << endl;
	    return;
	  }
	  float totalCorrection = crystalCorrectionFunction(nCryinclu - 1, e3x3);
	  
	  e3x3 *= totalCorrection;
	  et_s *= totalCorrection;
	  
	  S9Clu[nClu] = e3x3;
	  ptClu[nClu] =  et_s;
	  
	  S4Clu[nClu] *= totalCorrection;
	  e5x5 *= totalCorrection;
	  S25Clu[nClu] = e5x5;
	  
	} // check for doing energy re-calibration

	if(useClusterCrystalLimit_ && useClusterCrystalLimitLowPtOnly_  && (nCryinclu < lowCrystalLimit_ || nCryinclu > highCrystalLimit_)) {
	  //                                                                                                                      
	  // Cluster limits option was specified, there is no requirement for low pair pT, and this cluster is outside of the limits                                                                                                                             
	  // Cluster will be rejected                                                                                             
	  //                                                                                                                      
	  continue; // do not use this cluster, crystal number not in range
	  cout<<"-X-X-X My condition is not satisfied... You are fine... "<<endl;
	}
	

	//                                                                                                                    
	// Check the timing information                                                                                       
	//                                                                                                                    
	bool goodTimingCheck = true;

	if(doOneDimensionTimeCut_) {
	  if(oneDimensionTimeCut_ < 0) {
	    if(fabs(seedTimeClu[nClu]) < -oneDimensionTimeCut_) {
	      goodTimingCheck = false;
	    }
	  } // reverse cut                                                                                                    
	  if(doOneDimensionTimeCut_ > 0) {
	    if(fabs(seedTimeClu[nClu]) > oneDimensionTimeCut_) {
	      goodTimingCheck = false;
	    }
	  } // normal cut                                                                       
	} // check for doing one dimensional  timing cuts on the seed crystal of the cluster

	if(doTwoDimensionTimeCut_) {

	  //                                                                                                          
          // The default 2D cut has a base width of 20 ns and a minimum value of 8 ns                              
	  // The rescaled (better) 2D cut has these dimensions reduced by the factor twoDimensionTimeCutFactor           
	  //                                                                                                     
     
	  float energyInverse = 1.0/S9Clu[nClu];

	  float tCut = 4.0 - 10.8452*energyInverse + 14.9102*energyInverse*energyInverse -3.065*energyInverse*energyInverse*energyInverse;

	  if(S9Clu[nClu] > 1.123)
	    tCut = 4.0;  // minimum value for the default time cut                                                  
	  tCut *= twoDimensionTimeCutFactor_;  // rescale the size of the default 2D cut                             
	  
	  if(fabs(seedTimeClu[nClu]) > tCut) {
	    goodTimingCheck = false;
	  }
	  //                                                                                                           
	  // Check timing for clusters with 2 or more crystals                                                     
	  //                                                                                                            
	  if(doWeightedTimeCut_ && nCryinclu > 1 && nCryinclu <= 9) {
	    if(fabs(weightedTimeClu[nClu] - seedTimeClu[nClu]) > tCut) {
	      goodTimingCheck = false;
	    }
	  } // check weighted time for clusters with two or more crystals                                             
	} // check for doing two dimensional timing cuts on the seed crystal of the cluster                           

	if(goodTimingCheck) {
	  etaCluUncorrected[nClu] = etaForZEqualsZero;
	  double thetaUncorrected = 2.0*atan(exp(-etaCluUncorrected[nClu]));
	  zClu[nClu] = 100.0*rECal/tan(thetaUncorrected);  // Z value of cluster in cm                            
	  nClu++;
	} // accept this cluster as having good timing                                                                
      } // loop over seeds
      
    }            // rhEBpi0.valid() ends
    
    
    if(nClu > maxClustersPerJob) {
      maxClustersPerJob = nClu;
      eventMaxClustersPerJob = thisEvent;
      runMaxClustersPerJob = thisJob;
    }
    
    if(nCry > maxCryPerJob) {
      maxCryPerJob = nCry;
      eventMaxCryPerJob = thisEvent;
      runMaxCryPerJob = thisJob;
    }


    ///////////// For selecting events based on leading photon pt 
    double leadingPhotonPt = 0.0;

    for(int kCluster=0; kCluster<nClu; kCluster++) {

      float clustrEnr = S9Clu[kCluster];
      float clustrPt = ptClu[kCluster];
      float clustrS49 = S4Clu[kCluster]/clustrEnr;
      float clustrS1 = S1Clu[kCluster];
      float clustrEta = etaClu[kCluster];
      float clustrS25 = S25Clu[kCluster];

      if(clustrEnr > clustEnrCut && clustrPt > clustPtCut &&
         clustrS1 > clustS1Cut &&
         fabs(clustrEta) < 1.49 && (fabs(1.0 - clustrS25/clustrEnr) < clustS25Cut)) {

        if( clustrPt > leadingPhotonPt ) leadingPhotonPt = clustrPt;
      }
    }

    EventsLeadPhotonPt->Fill(leadingPhotonPt);

    if (cutByLeadingPhotonPt_ )
      {
        if( leadingPhotonPt < leadingPhotonPtMin_ ) return;
        if( leadingPhotonPt > leadingPhotonPtMax_ ) return;
      }

    ////////////////////////////////////////////////////////////////////
    
    for(int kCluster=0; kCluster<nClu; kCluster++) {
      
      float clustrEnr = S9Clu[kCluster];
      float clustrPt = ptClu[kCluster];
      float clustrS49 = S4Clu[kCluster]/clustrEnr;
      float clustrS1 = S1Clu[kCluster];
      float clustrEta = etaClu[kCluster];
      float clustrS25 = S25Clu[kCluster];

      if(clustrEnr > clustEnrCut && clustrPt > clustPtCut &&
	 clustrS1 > clustS1Cut &&
	 fabs(clustrEta) < 1.49 && (fabs(1.0 - clustrS25/clustrEnr) < clustS25Cut)) {

	PhotonClusterPt->Fill(clustrPt);      
	
      }
      // These cuts are used as the global cuts below for the pi0 mass histograms
      ShowerShapeCut->Fill(clustrS49);
      
      if(clustrEnr > clustEnrCut && clustrPt > clustPtCut &&
	 clustrS49 > clustS49Cut && clustrS1 > clustS1Cut &&
	 fabs(clustrEta) < 1.49 && (fabs(1.0 - clustrS25/clustrEnr) < clustS25Cut)) {
	
	if(currentAcceptedClusterNumber >= maximumNumberAcceptedCluster) {
	  cerr << "\n currentAcceptedClusterNumber is too large " << currentAcceptedClusterNumber << endl;
	  return;
	} // safety check on too many clusters
	
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].thisEvent = thisEvent;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].kCluster = kCluster;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].clustEta = clustrEta;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].clustPhi = phiClu[kCluster];
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].clustPt = clustrPt;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].clustEnr = clustrEnr;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].clustS49 = clustrS49;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].clustS25 = clustrS25;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].hfAllAngle = HFAngle;
	acceptedClusterInformationArray[currentBufferEventIndexCluster][currentAcceptedClusterNumber].nCryClu = nCryClu[kCluster]; 
		
	currentAcceptedClusterNumber++;
	
	if(currentAcceptedClusterNumber > maximumClustersInSingleEvent)
	  maximumClustersInSingleEvent = currentAcceptedClusterNumber;
	
      } // check for good cluster
      
    } // loop over good clusters=======================end loop filling array with current clusters=========================
    
    if(currentAcceptedClusterNumber >= minimumClustersPerEvent) {
      totalEventsWithClusters++;
      countAcceptedCluster[currentBufferEventIndexCluster] = currentAcceptedClusterNumber; // clusters accepted in the current event
      
      // Calculate same-event and mixed-event mass spectra
      
      for(int jCluster1=0; jCluster1<currentAcceptedClusterNumber; jCluster1++) {//=====loop over clusters in current event====
	
	Long64_t currentEvent = acceptedClusterInformationArray[currentBufferEventIndexCluster][jCluster1].thisEvent;
	
	float ptCluster1 = acceptedClusterInformationArray[currentBufferEventIndexCluster][jCluster1].clustPt;
	float etaCluster1 = acceptedClusterInformationArray[currentBufferEventIndexCluster][jCluster1].clustEta;
	float phiCluster1 = acceptedClusterInformationArray[currentBufferEventIndexCluster][jCluster1].clustPhi;
	float enrCluster1 = acceptedClusterInformationArray[currentBufferEventIndexCluster][jCluster1].clustEnr;
	
	float p1x = ptCluster1*cos(phiCluster1);
	float p1y = ptCluster1*sin(phiCluster1);
	float p1z = enrCluster1*cos(2.0*atan(exp(-etaCluster1)));
	
	double clust1Theta = 2.0*atan(exp(-etaCluster1));
	
	float xCluster1 = rECal*cos(phiCluster1);
	float yCluster1 = rECal*sin(phiCluster1);
	float zCluster1 = rECal/tan(clust1Theta);

	int nCryClu1 = acceptedClusterInformationArray[currentBufferEventIndexCluster][jCluster1].nCryClu; 
	
	for(int kBuffer=0; kBuffer<bufferDepth; kBuffer++) {//=======Loop over OTHER events====================
	  bool sameEventCheck = false;
	  if(currentEvent == eventNumberCluster[kBuffer]) {
	    sameEventCheck = true;
	  }
	  
	  int previousAcceptedNumber = countAcceptedCluster[kBuffer];
	  if(sameEventCheck && currentAcceptedClusterNumber != previousAcceptedNumber) {
	    cerr << "\n Programming error, same event cluster numbers are not the same";
	    cerr << ";  currentAcceptedClusterNumber " << currentAcceptedClusterNumber;
	    cerr << ",  previousAcceptedNumber " << previousAcceptedNumber;
	    cerr << ",  currentEvent = " << currentEvent;
	    cerr << endl;
	    return;
	  }
	  
	  for(int jCluster2=0; jCluster2<previousAcceptedNumber; jCluster2++) {//=============loop over clusters in kbuffer==
	    
	    Long64_t previousEvent = acceptedClusterInformationArray[kBuffer][jCluster2].thisEvent;
	    if(previousEvent == currentEvent && !sameEventCheck) {
	      cerr << "\n Programming error: trying to mix clusters with the same event number " << currentEvent;
	      cerr << ",  kBuffer = " << kBuffer << ", clusters in previous event " << previousAcceptedNumber;
	      for(unsigned int jBuffer=0; jBuffer<bufferDepth; jBuffer++) {
		cout << "\n jBuffer " << jBuffer << ",  eventNumber " << eventNumberCluster[jBuffer];
		cout << ", clusters in event " << countAcceptedCluster[jBuffer];
	      }
	      cerr << endl;
	      return;
	    }
	    
	    if(sameEventCheck && jCluster2<=jCluster1)
	      continue;
	    
	    if(sameEventCheck)
	      countSameEvents++;
	    
	    countMixedClusterExamined++;
	    
	    float ptCluster2 = acceptedClusterInformationArray[kBuffer][jCluster2].clustPt;
	    float etaCluster2 = acceptedClusterInformationArray[kBuffer][jCluster2].clustEta;
	    float enrCluster2 = acceptedClusterInformationArray[kBuffer][jCluster2].clustEnr;
	    float phiCluster2 = acceptedClusterInformationArray[kBuffer][jCluster2].clustPhi;
	    
	    float p2x = ptCluster2*cos(phiCluster2);
	    float p2y = ptCluster2*sin(phiCluster2);
	    float p2z = enrCluster2*cos(2.0*atan(exp(-etaCluster2)));
	    double clust2Theta = 2.0*atan(exp(-etaCluster2));
	    
	    float pxsum = p1x + p2x;
	    float pysum = p1y + p2y;
	    float pi0Pt = sqrt(pxsum*pxsum + pysum*pysum);

	    int nCryClu2 = acceptedClusterInformationArray[kBuffer][jCluster2].nCryClu;
	    //                                                                                                                    
	    // Check for cluster crystal limits                                                                                   
	    //                                                                                                                    
	    if(useClusterCrystalLimit_ && useClusterCrystalLimitLowPtOnly_  && pi0Pt < clusterCrystalLimitLowPtOnly_ &&
	       (nCryClu1 < lowCrystalLimit_ || nCryClu1 > highCrystalLimit_ ||
		nCryClu2 < lowCrystalLimit_ || nCryClu2 > highCrystalLimit_))
	      continue; // reject this pair because of the cluster crystal limits check  	    
	    
	    float pi0Phi = atan2(pysum, pxsum); 
	    float pzsum = p1z + p2z;
	    float totalMomentum = sqrt(pi0Pt*pi0Pt + pzsum*pzsum);
	    float pi0Theta = acos(pzsum/totalMomentum);
	    float pi0Eta = -log(tan(pi0Theta/2.));
	    
	    float xCluster2 = rECal*cos(phiCluster2);
	    float yCluster2 = rECal*sin(phiCluster2);
	    float zCluster2 = rECal/tan(clust2Theta);
	    
	    float dSeparationSquared = (xCluster2 - xCluster1)*(xCluster2- xCluster1) +
	      (yCluster2 - yCluster1)*(yCluster2- yCluster1) +
	      (zCluster2 - zCluster1)*(zCluster2- zCluster1);
	    
	    if(dSeparationSquared < minimumSeparationSquared) {
	      continue;
	    } // check for passing absolute minimum separation cut
	    
	    if(usePtDependentSeparation && pi0Pt > 0.0 ) {
	      // Parameterization result is in cm, but need meters for checking
	      double predictedMinimumDistance = 0.01*rescaleSeparationFactor*(aSeparationCoefficient/pi0Pt + bSeparationCoefficient/(pi0Pt*pi0Pt));
	      if(dSeparationSquared < predictedMinimumDistance*predictedMinimumDistance) {
		countSeparationPtCut++;
		continue;
	      } // check for passing pT dependent separation cut
	    }
	    
	    float pi0Energy = enrCluster1 + enrCluster2;
	    float combinedEventPi0Mass = sqrt(pi0Energy*pi0Energy - totalMomentum*totalMomentum);
	    
	    if(useClusterEnergyAsymmetryCut && combinedEventPi0Mass < maximumPi0MassForHistogram) {  // asymmetry cut for pi0 mass only
	      float clusterEnergySum = enrCluster2 + enrCluster1;
	      
	      if(clusterEnergySum <= 0.0) {
		cerr << "\n Reconstruction error, cluster energy sum = " << clusterEnergySum;
		cerr << "\n  Cluster1 energy = " <<  enrCluster1;
		cerr << "\n  Cluster2 energy = " <<  enrCluster2;
		cerr << endl;
		return;
	      } // safety check on cluster energy sum
	      float clusterEnergyAsymmetry = fabs(enrCluster2 - enrCluster1)/clusterEnergySum;
	      if(clusterEnergyAsymmetry >  clusterEnergyAsymmetryCut) {
		
		continue;  // skip this combination
	      }
	      
	    } // check on using cluster energy asymmetry cut
	    
	    
	    double cosOpenAngle = cos(clust1Theta)*cos(clust2Theta) + sin(clust1Theta)*sin(clust2Theta)*cos(phiCluster2 - phiCluster1);
	    float openAngle = acos(cosOpenAngle);
	    
	    if(combinedEventPi0Mass > 0.0 && combinedEventPi0Mass < maximumPi0MassForHistogram) {
	      
	      // Checks for combinations in the pi0 mass range
	      
	      bool failOpeningAngleCut = false;
	      if(useFixedOpeningAngleCut)
		failOpeningAngleCut = true;
	      
	      if(useFixedOpeningAngleCut) {
		
		// opening angle cut according to pT-Dependent parameterization
		
		float pTDependentOpenAngleCut = aOpenAngleCutParameter/pi0Pt + bOpenAngleCutParameter/(pi0Pt*pi0Pt);
		if(openAngle > pTDependentOpenAngleCut)
		  failOpeningAngleCut = false;
	      } // check on using pT-dependent opening angle parameterization
	      
	      if(failOpeningAngleCut) {
		continue; // skip this combination
	      } // check on passing opening angle cut, either same-event or mixed-event
	      
	    }// check on being  inside pi0 mass histogram region 
	    
	    if(pi0Pt < lowpi0PtCut_ || pi0Pt > highpi0PtCut_)
	      continue;
	    if(pi0Eta <= lowEtaLimit || pi0Eta >= highEtaLimit)
	      continue;

	    double effpi0 = effhisto_pi0->GetBinContent(effhisto_pi0->FindBin(pi0Pt,pi0Eta));
	    
	    if(combinedEventPi0Mass > 0.0 && combinedEventPi0Mass < maximumPi0MassForHistogram) {
	      if(sameEventCheck) {
		pi0MassHistSameEvent->Fill(combinedEventPi0Mass);
		pi0PhiTrueHist->Fill(pi0Phi);
		pi0EtaTrueHist->Fill(pi0Eta,1.0/effpi0);
		
		char histogramName1[200];
		char histogramName2[200];
		for(int kPt=0; kPt<bins1; kPt++) {
		  if(pi0Pt > NptBins_[kPt] && pi0Pt <= NptBins_[kPt+1])
		    {
		      sprintf(histogramName1, "pi0MassSameEventPtBin%d", kPt);
		      pi0MassHistSameEventvsPt[histogramName1]->Fill(combinedEventPi0Mass);
		      sprintf(histogramName2, "pi0pT%d", kPt);
		      pi0MeanPt[histogramName2]->Fill(pi0Pt,1.0/effpi0);
		    }
		}
	       
		if(combinedEventPi0Mass >=0.10 && combinedEventPi0Mass <= 0.17) {
		//		if(combinedEventPi0Mass >=0.090 && combinedEventPi0Mass <= 0.18) {
		//		if(combinedEventPi0Mass >=0.03 && combinedEventPi0Mass < 0.09) {
		//		if(combinedEventPi0Mass >=0.18 && combinedEventPi0Mass < 0.24) {
		//		if(combinedEventPi0Mass >=0.22 && combinedEventPi0Mass < 0.28) {
		  //Used to calculate the correlation function from the side-band
		  _pi0Spectrum->Fill(pi0Eta, pi0Pt, occ);
		  pi0PtTrueHist->Fill(pi0Pt);
		  		  
		  if(pi0HadronCorrelations_) {
		    TVector3 pvectorPi0;
		    pvectorPi0.SetPtEtaPhi(pi0Pt,pi0Eta,pi0Phi);
		    pVect_trg.push_back(pvectorPi0);
		  }
		  
		  ///// For di-pion correlations //////
		  /*		TVector3 pvector;
		    pvector.SetPtEtaPhi(pi0Pt,pi0Eta,pi0Phi);
		    
		    if(pi0Eta<=etaMax_ass_ && pi0Eta>=etaMin_ass_
		    && pi0Pt<=ptMax_ass_ && pi0Pt>=ptMin_ass_)
		    {
		    pVect_ass.push_back(pvector);
		    tPt_->Fill(pi0Pt);
		    tEta_->Fill(pi0Eta);
		    tPhi_->Fill(pi0Phi);
		    }*/
		  /////////////////////////////////////
		  
		}
		
		// For eta-hadron correlations 
		if(combinedEventPi0Mass >=0.4 && combinedEventPi0Mass < 0.6) {
		  if(etaHadronCorrelations_) {
		    TVector3 pvectorPi0;
		    pvectorPi0.SetPtEtaPhi(pi0Pt,pi0Eta,pi0Phi);
		    pVect_trg.push_back(pvectorPi0); }
		}
		
	      } // same event check
	      else {
		
		pi0MassHistMixed->Fill(combinedEventPi0Mass);
		
		char histogramName3[200];
		for(int kPt=0; kPt<bins1; kPt++) {
		  if(pi0Pt > NptBins_[kPt] && pi0Pt <= NptBins_[kPt+1])
		    {
		      sprintf(histogramName3, "pi0MassMixedEventPtBin%d", kPt);
		      pi0MassHistMixedEventvsPt[histogramName3]->Fill(combinedEventPi0Mass);
		    }
		}
		
	      } // check for mixed event mode
	    } // check histogram mass window
	    
	  } // loop over previous event clusters
	  
	} // loop over event buffer
	
      } // loop over current clusters
   
          
      currentBufferEventIndexCluster++;
      if(currentBufferEventIndexCluster == bufferDepth)
	currentBufferEventIndexCluster = 0;  // roll back to the start of the buffer for filling with the next event
      
      if(currentBufferEventIndexCluster >= bufferDepth) {
	cerr << "\n Programming error: attempting to set an index beyond the event limit in the buffer";
	cerr << endl;
	return;
      }
      
    }//// check for minimum number of clusters in the this event
 


    /////// Finally for pi0-hadron correlations //////////////////
    /////// Calculating the signal first ////////////////
    
    int nMult_trg = (int)pVect_trg.size();
    int nMult_ass = (int)pVect_ass.size();
    
    double nMult_trg_eff_pi0=0;

    for(int ntrg=0; ntrg<nMult_trg; ++ntrg)
      {
	TVector3 pvector_trg = (pVect_trg)[ntrg];
	double eta_trg = pvector_trg.Eta();
	double pt_trg = pvector_trg.Pt(); 

	if(doEffCorrections_){
	  float effCorrFactor = efficiencyCorrections(pt_trg, eta_trg, tHighPurityTracks_);
	  double effpi0 = effhisto_pi0->GetBinContent(effhisto_pi0->FindBin(pt_trg,eta_trg));
	  //	  cout<<"pT trig = "<<pt_trg<<'\t'<<"eta_trg"<<eta_trg<<endl;
	  //      cout<<"Efficiency correction factor = "<<effCorrFactor<<'\t'<<"Efficiency = "<<effpi0<<endl;
	  nMult_trg_eff_pi0 = nMult_trg_eff_pi0 + 1.0*effCorrFactor/effpi0;
	  //nMult_trg_eff_pi0 = nMult_trg_eff_pi0 + 1.0/effpi0;
	}

	else {
          double effpi0 = 1.0;
          float effCorrFactor = 1.0;
	  nMult_trg_eff_pi0 = nMult_trg_eff_pi0 + 1.0*effCorrFactor/effpi0;
        }
      }

    //	cout<<"Uncorrected multiplicity  = "<<nMult_trg<<'\t'<<"corrected efficiency multiplicity = "<<nMult_trg_eff_pi0<<endl;

    for(int ntrg=0; ntrg<nMult_trg; ++ntrg)
      {
	TVector3 pvector_trg = (pVect_trg)[ntrg];
	double eta_trg = pvector_trg.Eta();
	double phi_trg = pvector_trg.Phi();
	double pt_trg = pvector_trg.Pt(); 
	
	float effCorrFactor;
	double effpi0;

	if(doEffCorrections_){
	  effCorrFactor = efficiencyCorrections(pt_trg, eta_trg, tHighPurityTracks_);
	  // cout<<"The multiplicity of the track = "<<tHighPurityTracks_<<endl;
	  // cout<<"pT trig = "<<pt_trg<<'\t'<<"Correction factor = "<<effCorrFactor<<endl;
	  effpi0 = effhisto_pi0->GetBinContent(effhisto_pi0->FindBin(pt_trg,eta_trg));
	}
	else {
	  effpi0 = 1.0;
	  effCorrFactor = 1.0;
	}

	for(int nass=0; nass<nMult_ass; nass++)
	  {
	    TVector3 pvector_ass = (pVect_ass)[nass];
	    double eta_ass = pvector_ass.Eta();
	    double phi_ass = pvector_ass.Phi();
	    double pt_ass = pvector_ass.Pt();
	    
	    double effweight_ass;
	    if(doEffCorrections_) effweight_ass = effhisto->GetBinContent(effhisto->FindBin(eta_ass,pt_ass));
	    else effweight_ass = 1.0;

	    double deltaEta = eta_ass - eta_trg;
	    double deltaPhi = phi_ass - phi_trg;
	    if(deltaPhi > _pi) deltaPhi = deltaPhi - 2*_pi;
	    if(deltaPhi < -_pi) deltaPhi = deltaPhi + 2*_pi;
	    if(deltaPhi > -_pi && deltaPhi < -_pi/2.0) deltaPhi = deltaPhi + 2*_pi;
	    
	    if(deltaEta == 0 && deltaPhi == 0) continue;
	    
	    if(pi0HadronCorrelations_ || etaHadronCorrelations_) {
	      
	      char histogramName5[200];
	      for(int kPt=0; kPt<bins1; kPt++) {
		if(pt_trg > NptBins_[kPt] && pt_trg <= NptBins_[kPt+1]) //correction
		  {
		    sprintf(histogramName5, "hSignalPtBin%d", kPt);
		    
		    double weight = effCorrFactor/4.0/nMult_trg_eff_pi0/effpi0/effweight_ass;
		    hSignal[histogramName5]->Fill(fabs(deltaEta),fabs(deltaPhi),weight);
		    hSignal[histogramName5]->Fill(-fabs(deltaEta),fabs(deltaPhi),weight);
		    hSignal[histogramName5]->Fill(fabs(deltaEta),-fabs(deltaPhi),weight);
		    hSignal[histogramName5]->Fill(-fabs(deltaEta),-fabs(deltaPhi),weight);
		    hSignal[histogramName5]->Fill(fabs(deltaEta),2*_pi-fabs(deltaPhi),weight);
		    hSignal[histogramName5]->Fill(-fabs(deltaEta),2*_pi-fabs(deltaPhi),weight);
		  }
	      }
	    }
	    
	    if(diHadronCorrelations_) {
	      hSignal1->Fill(fabs(deltaEta),fabs(deltaPhi),1.0/4.0/nMult_trg/effweight_ass);
	      hSignal1->Fill(-fabs(deltaEta),fabs(deltaPhi),1.0/4.0/nMult_trg/effweight_ass);
	      hSignal1->Fill(fabs(deltaEta),-fabs(deltaPhi),1.0/4.0/nMult_trg/effweight_ass);
	      hSignal1->Fill(-fabs(deltaEta),-fabs(deltaPhi),1.0/4.0/nMult_trg/effweight_ass);
	      hSignal1->Fill(fabs(deltaEta),2*_pi-fabs(deltaPhi),1.0/4.0/nMult_trg/effweight_ass);
	      hSignal1->Fill(-fabs(deltaEta),2*_pi-fabs(deltaPhi),1.0/4.0/nMult_trg/effweight_ass); 
	    }
	  }
      }
    pVectVect_trg.push_back(pVect_trg);
    pVectVect_ass.push_back(pVect_ass);
    zvtxVect.push_back(zVertexEventSelected); 
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    /// mc truth
    isMC = !iEvent.isRealData();
    
    // get MC info from hiGenParticleCandidates 
    Handle<GenParticleCollection> hiGenParticles;
    if(isMC) iEvent.getByLabel("hiGenParticles", hiGenParticles);
    
    // get GEANT sim tracks and vertices (includes conversions)
    Handle<SimTrackContainer> simTracks_h;
    const SimTrackContainer* simTracks;
    if( isMC ) iEvent.getByLabel("g4SimHits", simTracks_h);
    simTracks = (simTracks_h.isValid()) ? simTracks_h.product() : 0;
    
    Handle<SimVertexContainer> simVert_h;
    const SimVertexContainer* simVertices;
    if( isMC ) iEvent.getByLabel("g4SimHits", simVert_h);
    simVertices = (simVert_h.isValid()) ? simVert_h.product() : 0;
    
    if( isMC ) 
      {
	nMC = nSIM = 0;
	int current_particle = -1;
	set<int> mothers;
	map<const GenParticle*, int> mapMC;
	map<const GenParticle*, int>::iterator mapMC_it;
	//int where_is_pi0_mother(0);  //debug
	int p_count(0);
	int motherIDMC_temp = -1;
	
	//cout << endl << "event" << endl;
	for (GenParticleCollection::const_iterator p = hiGenParticles->begin(); p != hiGenParticles->end(); ++p, ++p_count) 
	  {
	    if ( nMC >= (nMaxMC-1) ) continue;
	    
	    // looking for mother particle
	    motherIDMC_temp = -1;
	    //where_is_pi0_mother = -1; // debug
	    if (p->numberOfMothers() > 0) 
	      {
		const Candidate * mom = p->mother();
		for (size_t j = 0; j != hiGenParticles->size(); ++j) 
		  {
		    const Candidate * ref = &((*hiGenParticles)[j]);
		    if (mom==ref)
		      {
			motherIDMC_temp = j; 
			//if(mom->pdgId()==kPi0)//debug
			//   where_is_pi0_mother = j; // debug
		      }
		  }
	      } 
	    
	    // Neutral particles kept with >100 MeV 
	    if ( p->status()==1  && p->pt()>0.100 )//|| (p->charge()!=0 && p->pt()>0.075)) ) 
	      {
		pdgIdMC[nMC] = p->pdgId();
		statusMC[nMC] = p->status();
		ptMC[nMC] = p->pt();
		eMC[nMC] = p->energy();
		etaMC[nMC] = p->eta(); 
		phiMC[nMC] = p->phi(); 
		mapMC[&(*p)] = nMC;
		convertedMC[nMC] = false;
		motherIDMC[nMC] = -1; //((const hiGenParticle*)p->mother())->pdgId(); 
		motherIndexMC[nMC] = -1; //hic::check
		current_particle = nMC;
		++nMC; 
		
		// if stable photon/electron, find parent
		if (p->status() == 1 && motherIDMC_temp != -1 && (p->pdgId() == kPhoton || fabs(p->pdgId()) == kElectron ) ) //|| p->pdgId() == 221)) // 221=eta0 
		  {
		    const GenParticle *mom = (const GenParticle*)p->mother();
		    
		    // saving the mother pdgId 
		    motherIDMC[current_particle] = mom->pdgId();
		    
		    if ( mom->status() == 2 &&  (mom->pdgId()<81 || mom->pdgId()>100)) // avoid to store partons and MC internal
		      {
			// if the mother particle is found for the first time
			if(mothers.find(motherIDMC_temp) == mothers.end())
			  {
			    mothers.insert(motherIDMC_temp);
			    if (nMC>=nMaxMC) continue;
			    pdgIdMC[nMC] = mom->pdgId();
			    statusMC[nMC] = mom->status();
			    ptMC[nMC] = mom->pt();
			    eMC[nMC] = mom->energy();
			    etaMC[nMC] = mom->eta();
			    phiMC[nMC] = mom->phi(); 
			    mapMC[mom] = nMC;
			    //motherIndexMC[nMC-1] = nMC;
			    ++nMC;
			  }
			//else
			//{
			//motherIndexMC[nMC-1] = mapMC.find(mom)->second;
			//}
		      }
		    
		    mapMC_it =  mapMC.find(mom);
		    if(mapMC_it != mapMC.end())
		      motherIndexMC[current_particle] = mapMC_it->second;
		    //cout << "   my_pi0::" << motherIndexMC[current_particle] << endl;
		  } // stable particle has parents 
	      } // particle selection
	  } // loop over particles
	
	
	//----- Figure out the particle decays in tracker volume  ------
	
	// Vertices only return trackID of their parent SimTrack
	// Figure out the mapping from trackID to SimTrack
	map<unsigned int, const SimTrack*> trackMap;
	for (SimTrackContainer::const_iterator iSim = simTracks->begin(); iSim != simTracks->end(); ++iSim) 
	  {
	    if (!iSim->noVertex()) 
	      {
		assert(trackMap.find(iSim->trackId())==trackMap.end());
		trackMap[iSim->trackId()] = &(*iSim);
	      }
	  }
	
	
	// Find all SimTracks that come from decays before the ECAL
	// and find their parent SimTracks
	map<const SimTrack*, const SimTrack*> promptParent; // daughter->mother
	map<const SimTrack*, set<const SimTrack*> > promptDecays; // m->ds
	map<const SimTrack*, const SimVertex*> promptVertex; // daughter->vertex
	
	for (SimTrackContainer::const_iterator iSim = simTracks->begin(); iSim != simTracks->end(); ++iSim) 
	  {
	    if (!iSim->noVertex()) 
	      {
		// Find the parent vertex and see if it classifies as an early decay
		// Exclude the primary vertex (noParent)
		SimVertex const& vtx = (*simVertices)[iSim->vertIndex()];
		if (!vtx.noParent() && vtx.position().Rho() < 129 && fabs(vtx.position().z()) < 304) 
		  {
		    // Find parent SimParticle that produced this vertex
		    // vtx->parentIndex is NOT a vector index :( so use trackMap
		    assert(trackMap.find(vtx.parentIndex())!=trackMap.end());
		    const SimTrack* p = trackMap[vtx.parentIndex()];
		    promptParent[&(*iSim)] = p;
		    promptDecays[p].insert(&(*iSim));
		    promptVertex[&(*iSim)] = &vtx;
		  } // early decay
	      } // has vertex
	  } // for simTracks
	
	// Find grandparent SimTracks all the way up the chain
	map<const SimTrack*, const SimTrack*> chainParents;// g.daughter->grandma
	map<const SimTrack*, set<const SimTrack*> > chainDecays; // gm->gds
	
	for (map<const SimTrack*, const SimTrack*>::const_iterator iSim = promptParent.begin(); iSim != promptParent.end(); ++iSim) 
	  {
	    // Check that the SimTrack has no daughters itself (=grandchild)
	    if (promptDecays.find(iSim->first)==promptDecays.end())
	      {
		// Find the first SimTrack in the parentage chain (=grandparent)
		const SimTrack *p = iSim->second;
		while (promptParent.find(p) != promptParent.end())
		  p = promptParent[p];
		chainParents[iSim->first] = p;
		chainDecays[p].insert(iSim->first);
	      } // is grandchild
	  } // for promptParent
	
	// Associate grandParents to hiGenParticles
	map<const GenParticle*, const SimTrack*> decayedSims;
	map<const SimTrack*, const GenParticle*> decayedGens;
	
	for (map<const SimTrack*, set<const SimTrack*> >::const_iterator iSim = chainDecays.begin(); iSim != chainDecays.end(); ++iSim) 
	  {
	    if (iSim->first->noGenpart()) 
	      continue;
	    
	    // Make sure the decay chain wasn't already pruned out
	    if (promptDecays.find(iSim->first)!=promptDecays.end() && promptDecays[iSim->first].size()!=0) 
	      {
		// NB: genpartIndex offset by 1
		const GenParticle* iGen =
		  &(*hiGenParticles)[iSim->first->genpartIndex()-1];
		assert(iGen->pdgId()==iSim->first->type());
		decayedSims[iGen] = iSim->first;
		decayedGens[iSim->first] = iGen;
	      }
	  } // for chainParents 
	
      } // if isMC 
    else
      {
	nMC = 1;
	pdgIdMC[0] = -99999;
	statusMC[0] = -99999;
	motherIndexMC[0] = -99999;
	motherIDMC[0] = -99999;
	ptMC[0] = -99999;
	eMC[0] = -99999;
	etaMC[0] = -99999;
	phiMC[0] = -99999;
	convertedMC[0] = 0;
	
	nSIM = 1;
	pdgIdSIM[0] = -99999;
	statusSIM[0] = -99999;
	motherGenIndexSIM[0] = -99999;
	ptSIM[0] = -99999;
	eSIM[0] = -99999;
	etaSIM[0] = -99999;
	phiSIM[0] = -99999;
	rSIM[0] = -99999;
	zSIM[0] = -99999;
      }
    
}



// Testing - Ravi - BEGIn
void myClass::beginJob(){
	cout << "From myClass" << endl;
}

	void analyze(const edm::Event& e, const edm::EventSetup& c){
		cout << "from myClass - analyze" << endl;
}
// Testing - Ravi - End

// ------------ method called once each job just before starting event loop  ------------
void EcalFlowNtp::beginJob()
{

  // content
  // xtal
  // e, pt, time, eta, phi, flag, 
  // 
  // clus
  // e, time, eta, phi, eta_cms, phi_cms, isolation, s1, s4, s9, indici 9 cristalli
  // 
  // pi0
  // inv mass, e, pt, eta, phi, eta_cms, phi_cms, indice 2 clus

  // GENERAL block branches

  maxTracksPerJob = -1;
  maxClustersPerJob = -1;
  maxCryPerJob = -1;
  maxPi0PerJob = -1;

  eventMaxTracksPerJob = -1;
  eventMaxClustersPerJob = -1;
  eventMaxCryPerJob = -1;
  eventMaxPi0PerJob = -1;

  runMaxTracksPerJob = -1;
  runMaxClustersPerJob = -1;
  runMaxCryPerJob = -1;
  runMaxPi0PerJob = -1;

  totalEventsThisJob = 0;
  hotChannelSeedsDiscarded = 0;
  hotChannelSatellitesDiscarded = 0;

  lowCentralityCut = 0;
  highCentralityCut = 32;
  eventsInsideCentralityWindow = 0;
  eventsOutsideCentralityWindow = 0;

  TFile * file = new TFile("/home/janjamrk/CMSSW_5_3_8_HI_patch2/src/Pi0Analysis/EcalFlowNtp/pPbRun2013/PAHighPt/pbsCrab/TrackCorrections_HIJING_538_OFFICIAL_Mar24.root","r");
  //  edm::FileInPath fip("TrackCorrections_HIJING_538_OFFICIAL_Mar24.root");
  //  TFile f(fip.fullPath().c_str(),"READ");
  effhisto = (TH2F*)file->Get("rTotalEff3D");

  TFile * file1 = new TFile("/home/janjamrk/CMSSW_5_3_8_HI_patch2/src/Pi0Analysis/EcalFlowNtp/pPbRun2013/PAHighPt/pbsCrab/pi0EfficiencyWidth0.030_HiPurity120-260_S4To9_0.74_S25To9_1.0_Date20140624.root","r");
  //  edm::FileInPath fip1("efficiencyPi0.root");
  // TFile ff(fip1.fullPath().c_str(),"READ");
  effhisto_pi0 = (TH2D*)file1->Get("efficiencyPi0Yields");

}

// ------------ method called once each job just after ending the event loop  ------------
void EcalFlowNtp::endJob() {

  ////////////////// Calculating background for pi0-hadron correlations //////////
  int nevttotal_trg = (int)pVectVect_trg.size();
  int nevttotal_ass = (int)pVectVect_ass.size();

  cout << "\n\n EcalFlowNTP::endJob() <I>  nevttotal_trg = " << nevttotal_trg;
  cout << ",  nevttotal_ass = " << nevttotal_ass;
  cout << "\n Will check the Z-vertex differences";
  cout << endl;

  unsigned int nBackgroundFill = 0;
  if(nevttotal_ass > 1 && nevttotal_trg > 1 && totalEventsThisJob > 1) {

    unsigned int countAcceptedTriggerEvents = 0;
    bool *acceptedTriggerEvents = new bool [nevttotal_trg];

    // Look at Z-Vertex differences                                                                     
    for(int nevt_trg=0; nevt_trg<nevttotal_trg; nevt_trg++) {
      acceptedTriggerEvents[nevt_trg] = false;
      for(int nevt_ass=0; nevt_ass<nevttotal_ass; nevt_ass++) {

        if(nevt_trg == nevt_ass)
          continue;  // don't use the same event         
	if(fabs((zvtxVect)[nevt_trg] - (zvtxVect)[nevt_ass])<=2.0) {
          acceptedTriggerEvents[nevt_trg] = true;
          countAcceptedTriggerEvents++;
          break; // found at least one partner event with a close enough Z vertex                                      
        }
      } // loop over associated events              
    } // loop over trigger events                                                                     
    cout << "\n Number of trigger events with at least one partner event = " << countAcceptedTriggerEvents;
    cout << endl;

    for(int nevt_trg=0; nevt_trg<nevttotal_trg; nevt_trg++) {

      if(!acceptedTriggerEvents[nevt_trg])
	continue;  // skip this trigger event which has no partner event close enough in Z 
      nBackgroundFill++;

      int countGoodAssociated = 0;

      for(int nevt_ass=0; nevt_ass<nevttotal_ass; nevt_ass++) {
        if(nevt_trg == nevt_ass) { // check if the random trigger event matches this assocated event   
          continue;    // go to the next associated track         
        }

	if(fabs((zvtxVect)[nevt_trg] - (zvtxVect)[nevt_ass])>2.0){  // check if the Z vertex of the trigger and associated are separated by more than 2 cm
        continue;    // go to the next associated event                                                                             
        }

	countGoodAssociated++;
      }
      
      if(countGoodAssociated < 1) {
        cout << "\n For nevt_trg " << nevt_trg << " the number of good associated events = " << countGoodAssociated << endl;
        continue;
      }

      int takeRandomInterval = 1;
      if(countGoodAssociated > 10)
        takeRandomInterval = countGoodAssociated/10 + 1 ;
      
      int takeAssociated = 0;
      for(int nevt_ass=0; nevt_ass<nevttotal_ass; nevt_ass += takeRandomInterval) {
	
        if(nevt_trg == nevt_ass) { // check if the random trigger event matches this assocated event                               
          continue;    // go to the next associated track                                                                          
        }
	
        if(fabs((zvtxVect)[nevt_trg] - (zvtxVect)[nevt_ass])>2.0){  // check if the Z vertex of the trigger and associated are separated by more than 2 cm                                                                
	  continue;    // go to the next associated event                                            
	    }

        takeAssociated++;
        if(takeAssociated > 10)
          break;
	
	_DeltazVertex->Fill((zvtxVect)[nevt_trg] - (zvtxVect)[nevt_ass]);
	
	vector<TVector3> pVectTmp_trg = (pVectVect_trg)[nevt_trg];
	vector<TVector3> pVectTmp_ass = (pVectVect_ass)[nevt_ass];
	int nMult_trg1 = pVectTmp_trg.size();
	int nMult_ass1 = pVectTmp_ass.size();

	double nMult_trg_pi0_eff=0;

	for(int ntrg=0; ntrg<nMult_trg1; ++ntrg)
	  {
	    TVector3 pvectorTmp_trg = pVectTmp_trg[ntrg];
	    double eta_trg = pvectorTmp_trg.Eta();
	    double pt_trg = pvectorTmp_trg.Pt();

	    float effCorrFactor;
	    double effpi0;

	    if(doEffCorrections_){
	      effCorrFactor = efficiencyCorrections(pt_trg, eta_trg, tHighPurityTracks_);
	      effpi0 = effhisto_pi0->GetBinContent(effhisto_pi0->FindBin(pt_trg,eta_trg));
	      nMult_trg_pi0_eff = nMult_trg_pi0_eff + 1.0*effCorrFactor/effpi0;
	      //	      nMult_trg_pi0_eff = nMult_trg_pi0_eff + 1.0/effpi0;
	    }
	    else {
	      effpi0 = 1.0;
	      effCorrFactor = 1.0;
	      nMult_trg_pi0_eff = nMult_trg_pi0_eff + 1.0*effCorrFactor/effpi0;
	    }

	  }
	
	double nMult_ass_ch_eff=0;
	for(int nass=0; nass<nMult_ass1; ++nass)
	  {
	    
	    TVector3 pvectorTmp_ass = pVectTmp_ass[nass];
	    double eta_ass = pvectorTmp_ass.Eta();
	    double pt_ass = pvectorTmp_ass.Pt();

	    double effweight_ass;
	    if(doEffCorrections_) effweight_ass = effhisto->GetBinContent(effhisto->FindBin(eta_ass,pt_ass));
	    else effweight_ass = 1.0;	    

	    nMult_ass_ch_eff = nMult_ass_ch_eff + 1.0/effweight_ass;

	  }

	for(int ntrg=0; ntrg<nMult_trg1; ++ntrg)
	  {
	    TVector3 pvectorTmp_trg = pVectTmp_trg[ntrg];
	    double eta_trg = pvectorTmp_trg.Eta();
	    double phi_trg = pvectorTmp_trg.Phi();
	    double pt_trg = pvectorTmp_trg.Pt();

	    float effCorrFactor;
	    double effpi0;

	    if(doEffCorrections_){
	      effCorrFactor = efficiencyCorrections(pt_trg, eta_trg, tHighPurityTracks_);
               effpi0 = effhisto_pi0->GetBinContent(effhisto_pi0->FindBin(pt_trg,eta_trg));
            }
            else {
              effpi0 = 1.0;
              effCorrFactor = 1.0;
            }
	    
	    for(int nass=0; nass<nMult_ass1; ++nass)
	      {
		
		TVector3 pvectorTmp_ass = pVectTmp_ass[nass];
		double eta_ass = pvectorTmp_ass.Eta();
		double phi_ass = pvectorTmp_ass.Phi();
		double pt_ass = pvectorTmp_ass.Pt();

		double effweight_ass;
		if(doEffCorrections_) effweight_ass = effhisto->GetBinContent(effhisto->FindBin(eta_ass,pt_ass));
		else effweight_ass = 1.0;

		double deltaEta = eta_ass - eta_trg;
		double deltaPhi = phi_ass - phi_trg;
		if(deltaPhi > _pi) deltaPhi = deltaPhi - 2*_pi;
		if(deltaPhi < -_pi) deltaPhi = deltaPhi + 2*_pi;
		if(deltaPhi > -_pi && deltaPhi < -_pi/2.0) deltaPhi = deltaPhi + 2*_pi;
		
		if(deltaEta == 0 && deltaPhi == 0) continue;
		
		if(pi0HadronCorrelations_ || etaHadronCorrelations_) {

		  char histogramName6[200];
		  for(int kPt=0; kPt<bins1; kPt++) {
		    if(pt_trg > NptBins_[kPt] && pt_trg <= NptBins_[kPt+1])
		      {
			sprintf(histogramName6, "hBackgroundPtBin%d", kPt);

			//			hBackground[histogramName6]->Fill(deltaEta,deltaPhi,1.0/nMult_trg1/nMult_ass1);
			//hBackground[histogramName6]->Fill(deltaEta,2*_pi - deltaPhi,1.0/nMult_trg1/nMult_ass1);
			hBackground[histogramName6]->Fill(fabs(deltaEta),fabs(deltaPhi),1.0*effCorrFactor/nMult_trg_pi0_eff/effpi0/effweight_ass);
			hBackground[histogramName6]->Fill(-fabs(deltaEta),fabs(deltaPhi),1.0*effCorrFactor/nMult_trg_pi0_eff/effpi0/effweight_ass);
			hBackground[histogramName6]->Fill(fabs(deltaEta),-fabs(deltaPhi),1.0*effCorrFactor/nMult_trg_pi0_eff/effpi0/effweight_ass);
			hBackground[histogramName6]->Fill(-fabs(deltaEta),-fabs(deltaPhi),1.0*effCorrFactor/nMult_trg_pi0_eff/effpi0/effweight_ass);
			hBackground[histogramName6]->Fill(fabs(deltaEta),2*_pi - fabs(deltaPhi),1.0*effCorrFactor/nMult_trg_pi0_eff/effpi0/effweight_ass);
			hBackground[histogramName6]->Fill(-fabs(deltaEta),2*_pi - fabs(deltaPhi),1.0*effCorrFactor/nMult_trg_pi0_eff/effpi0/effweight_ass);
		      }
		  }
		}

		// Uncomment the above two line for analyzing pPb data
		
       		if(diHadronCorrelations_) {
		  hBackground1->Fill(fabs(deltaEta),fabs(deltaPhi),1.0/nMult_trg1/nMult_ass1/effweight_ass);
		  hBackground1->Fill(-fabs(deltaEta),fabs(deltaPhi),1.0/nMult_trg1/nMult_ass1/effweight_ass);
		  hBackground1->Fill(fabs(deltaEta),-fabs(deltaPhi),1.0/nMult_trg1/nMult_ass1/effweight_ass);
		  hBackground1->Fill(-fabs(deltaEta),-fabs(deltaPhi),1.0/nMult_trg1/nMult_ass1/effweight_ass);
		  hBackground1->Fill(fabs(deltaEta),2*_pi - fabs(deltaPhi),1.0/nMult_trg1/nMult_ass1/effweight_ass);
		  hBackground1->Fill(-fabs(deltaEta),2*_pi - fabs(deltaPhi),1.0/nMult_trg1/nMult_ass1/effweight_ass);
		}
		
	      }
	  }
      }
    }
      
      delete [] acceptedTriggerEvents;
      
    } // Check for event-mixing for more than one event. 
    cout << endl << "--- RESULTS ---" << endl;
    
    cout << "\n  Currently with pi0 mixed event entries = " << pi0MassHistMixed->GetEntries();
    cout << ",  mixed clusters examined " << countMixedClusterExamined;
    cout << ",  events containing at least one cluster " << totalEventsWithClusters++;
    
    cout << "\n Maximum number of HI tracks for run " << runMaxTracksPerJob << " is " << maxTracksPerJob << " at event " << eventMaxTracksPerJob;
    cout << "\n Maximum number clusters found for run " << runMaxClustersPerJob << " is "  << maxClustersPerJob  << " at event " << eventMaxClustersPerJob;
    cout << "\n Maximum number crystals found for run " << runMaxCryPerJob << " is " << maxCryPerJob << " at event " << eventMaxCryPerJob;
    cout << "\n Total events this run = " << totalEventsThisJob << " with accumulated events for all runs = " << totalEventsAllJobs;
    cout << "\n Number of events with trigger particle (nevttotal_trg) = " << nevttotal_trg;
    cout << "\n Number of events with associated particle (nevttotal_ass) = " << nevttotal_ass;
    cout << "\n Number of background fills = " << nBackgroundFill;
    cout << endl;
  
}

void EcalFlowNtp::initHistos(const edm::Service<TFileService> & fs)
{

  TH1D::SetDefaultSumw2();
  TFileDirectory chTrkQA = fs->mkdir( "ChargedParticles" );

  tEvents_ = chTrkQA.make<TH1F>("tEvents_","",1,0,1);

  tVertexX_ = chTrkQA.make<TH1F>("tVerteX_", "", 100, -1, 1);
  tVertexY_ = chTrkQA.make<TH1F>("tVerteY_", "", 100, -1, 1);
  tVertexZ_ = chTrkQA.make<TH1F>("tVerteZ_", "", 100, -30, 30);

  tHit_ = chTrkQA.make<TH1I>("tHit_", "Tracks by number of valid hits", 31, 0, 30);
  tHit_->GetXaxis()->SetTitle("Number of valid hits");

  tPt_ = chTrkQA.make<TH1F>("tPt_", "p_{T} spectrum", 100, 0, 20);
  tPt_->GetXaxis()->SetTitle("p_{T} (GeV/c)");

  EventsLeadTrack_ =  chTrkQA.make<TH1F>("EventsLeadTrack_", "Number of events by leading track pt", 100, 0, 100);
  EventsLeadTrack_->GetXaxis()->SetTitle("p_{T} (GeV/c)");

  tEta_ = chTrkQA.make<TH1F>("tEta_", "Pseudorapidity distribution", 50, -2.5, 2.5);
  tEta_->GetXaxis()->SetTitle("#eta distribution of the tracks");

  tPhi_ = chTrkQA.make<TH1F>("tPhi_", "Azimuthal distribution",100, -3.15, 3.15);
  tPhi_->GetXaxis()->SetTitle("#phi distribution of the tracks");

  tPt_trg_ = chTrkQA.make<TH1F>("tPt_trg_", "p_{T} spectrum", 100, 0, 20);
  tPt_trg_->GetXaxis()->SetTitle("p_{T} (GeV/c) of the trigger particle");

  tEta_trg_ = chTrkQA.make<TH1F>("tEta_trg_", "Pseudorapidity distribution", 50, -2.5, 2.5);
  tEta_trg_->GetXaxis()->SetTitle("#eta distribution of the tracks");

  tPhi_trg_ = chTrkQA.make<TH1F>("tPhi_trg_", "Azimuthal distribution",100, -3.15, 3.15);
  tPhi_trg_->GetXaxis()->SetTitle("#phi distribution of the tracks");

  tDz_ = chTrkQA.make<TH1F>("tDz_", "Track quality variable",100, -10.0, 10.0);
  tDz_->GetXaxis()->SetTitle("d_{z}/#sigma(d_{z})");
  tDxy_ = chTrkQA.make<TH1F>("tDxy_", "Track quality variable",100, -10.0, 10.0);
  tDxy_->GetXaxis()->SetTitle("d_{xy}/#sigma(d_{xy})");

  tPtQA_ = chTrkQA.make<TH1F>("tPtQA_", "Track quality variable",100, -0.5, 0.5);
  tPtQA_->GetXaxis()->SetTitle("sigma(p_{T}/p_{T})");

  tHPTracks_ = chTrkQA.make<TH1F>("tHPTracks_","",750, 0, 750);
  tAllTracks_ = chTrkQA.make<TH1F>("tAllTracks_","",750, 0, 750);

  tEtvsHiTracksPlus_ = chTrkQA.make<TH2F>("tEtvsHiTracksPlus_", "E_{T}+ in HF vs HI tracks",400, 0.0, 400.0, 300, 0.0, 300.0);
  tEtvsHiTracksPlus_->GetXaxis()->SetTitle("Number of pPb tracks");
  tEtvsHiTracksPlus_->GetYaxis()->SetTitle("E_{T}+ in HF [GeV]");

  tEtvsHiTracksMinus_ = chTrkQA.make<TH2F>("tEtvsHiTracksMinus_", "E_{T}- in HF vs HI tracks",400, 0.0, 400.0, 300, 0.0, 300.0);
  tEtvsHiTracksMinus_->GetXaxis()->SetTitle("Number of pPb tracks");
  tEtvsHiTracksMinus_->GetYaxis()->SetTitle("E_{T}- in HF [GeV]");

  tEtvsHPHiTracksPlus_ = chTrkQA.make<TH2F>("tEtvsHPHiTracksPlus_", "E_{T}+ in HF vs HP HI tracks",400, 0.0, 400.0, 300, 0.0, 300.0);
  tEtvsHPHiTracksPlus_->GetXaxis()->SetTitle("Number of pPb HP tracks");
  tEtvsHPHiTracksPlus_->GetYaxis()->SetTitle("E_{T}+ in HF [GeV]");

  tEtvsHPHiTracksMinus_ = chTrkQA.make<TH2F>("tEtvsHPHiTracksMinus_", "E_{T}- in HF vs HP HI tracks",400, 0.0, 400.0, 300, 0.0, 300.0);
  tEtvsHPHiTracksMinus_->GetXaxis()->SetTitle("Number of HP pPb tracks");
  tEtvsHPHiTracksMinus_->GetYaxis()->SetTitle("E_{T}- in HF [GeV]");

  TFileDirectory pi0Related = fs->mkdir( "NeutralPions" );

  ShowerShapeCut = pi0Related.make<TH1D>("ShowerShapeCut", "Shower-shape cut", nMassBins, 0.0, 2.0);
  ShowerShapeCut->SetXTitle("S4/S9 ratio");
  ShowerShapeCut->SetYTitle("# of entries");

  pi0MassHistSameEvent = pi0Related.make<TH1D>("pi0MassHistSameEvent", "#pi^{0} Peak in pPb Same Events", nMassBins, 0.0, maximumPi0MassForHistogram);
  pi0MassHistSameEvent->SetXTitle("#gamma#gamma Invariant mass (GeV/c^{2})");
  pi0MassHistSameEvent->SetYTitle(countsPerBin);

  char histogramName[200];
  char histogramTitle[200];

  char histogramName1[200];
  char histogramTitle1[200];

  char histogramName2[200];
  char histogramTitle2[200];

  for(int kPt=0; kPt<bins1; kPt++) {
    sprintf(histogramName, "pi0MassSameEventPtBin%d", kPt);
    sprintf(histogramTitle, "Same-event #pi^{0} for %5.2f < p_{T} < %5.2f ", NptBins_[kPt], NptBins_[kPt+1]);
    pi0MassHistSameEventvsPt[histogramName] = pi0Related.make<TH1D>(histogramName, histogramTitle, nMassBins, 0.0, maximumPi0MassForHistogram);

    sprintf(histogramName2, "pi0MassMixedEventPtBin%d", kPt);
    sprintf(histogramTitle2, "Mixed-event #pi^{0} for %5.2f < p_{T} < %5.2f ", NptBins_[kPt], NptBins_[kPt+1]);
    pi0MassHistMixedEventvsPt[histogramName2] = pi0Related.make<TH1D>(histogramName2, histogramTitle2, nMassBins, 0.0, maximumPi0MassForHistogram);

  }

  for(int kPt=0; kPt<bins1; kPt++) {
    sprintf(histogramName1, "pi0pT%d", kPt);
    sprintf(histogramTitle1, "Mean #pi^{0} p_{T} for %5.2f < p_{T} < %5.2f ", NptBins_[kPt], NptBins_[kPt+1]);
    pi0MeanPt[histogramName1] = pi0Related.make<TH1D>(histogramName1, histogramTitle1, nMassBins, NptBins_[kPt], NptBins_[kPt+1]);

  }

  pi0MassHistMixed = pi0Related.make<TH1D>("pi0MassHistMixed", "#pi^{0} Peak in pPb Mixed Events", nMassBins, 0.0, maximumPi0MassForHistogram);
  pi0MassHistMixed->SetXTitle("#gamma#gamma Invariant mass (GeV/c^{2})");
  pi0MassHistMixed->SetYTitle(countsPerBin);

  pi0EtaTrueHist = pi0Related.make<TH1D>("pi0EtaTrueHist", "True #pi^{0} #eta distribution", nBinsOneD, -1.5, +1.5);
  pi0EtaTrueHist->SetXTitle("#eta");

  pi0PhiTrueHist = pi0Related.make<TH1D>("pi0PhiTrueHist", "True #pi^{0} azimuthal distribution", nBinsOneD, -3.3, +3.3);
  pi0PhiTrueHist->SetXTitle("Azimuthal angle #phi (radians)");

  pi0PtTrueHist = pi0Related.make<TH1D>("pi0PtTrueHist", "True #pi^{0} p_{T}", ptBins_.size()-1, &ptBins_[0]);
  pi0PtTrueHist->SetXTitle("Transverse momentum p_T (GeV/c)");

  PhotonClusterPt = pi0Related.make<TH1D>("PhotonClusterPt", "Photon cluster p_{T}", 100, 0, 100);
  PhotonClusterPt->SetXTitle("Transverse momentum p_T (GeV/c)");

  EventsLeadPhotonPt = pi0Related.make<TH1D>("EventsLeadPhotonPt", "Leading Photon p_{T}", 100, 0, 100);
  EventsLeadPhotonPt->SetXTitle("Transverse momentum p_T (GeV/c)");

  PFPhotonPt = pi0Related.make<TH1D>("PFPhotonPt", "PF Photon p_{T}", 100, 0, 100);
  PFPhotonPt->SetXTitle("Transverse momentum p_T (GeV/c)");

  SwissCrossCut = pi0Related.make<TH1D>("SwissCrossCut", "Swiss cross cut", 100, 0.2, 1.2);
  SwissCrossCut->SetXTitle("1 - E4/E1");

  Timing = pi0Related.make<TH1D>("Timing", "RecHit timing [ns]", 100, 0., 80.);
  Timing->SetXTitle("RecHit timing [ns]");

  TimingInClustering = pi0Related.make<TH1D>("TimingInClustering", "RecHit timing in clustering [ns]", 100, 0., 80.);
  Timing->SetXTitle("RecHit timing in clustering [ns]");

  _pi0Spectrum = pi0Related.make<TH3F>("_pi0Spectrum", ";#eta;p_{T};occ var",
				       etaBins_.size()-1, &etaBins_[0],
				       NptBins_.size()-1, &NptBins_[0],
				       occBins_.size()-1, &occBins_[0]);

  _DeltazVertex = pi0Related.make<TH1D>("_DeltazVertex","", 100, -2.5, 2.5);
  _DeltazVertex->SetXTitle("#Delta z");
  _DeltazVertex->SetYTitle("Number of events");

  
  ////// pi-Had 2D correlation function
  TFileDirectory pi0HadCorrFunc = fs->mkdir( "pi0HadCorrFunc" );

  char histogramName3[200];
  char histogramTitle3[200];

  char histogramName4[200];
  char histogramTitle4[200];

  for(int kPt=0; kPt<bins1; kPt++) {
    sprintf(histogramName3, "hSignalPtBin%d", kPt);
    sprintf(histogramTitle3, "Same-event #pi^{0}-h 2D correlation function for %5.2f < p_{T} < %5.2f ", NptBins_[kPt], NptBins_[kPt+1]);
    hSignal[histogramName3] = pi0HadCorrFunc.make<TH2D>(histogramName3, histogramTitle3, 32+1,-5.0-0.15,5.0+0.15,32-1,-_pi/2+_pi/32,3*_pi/2-_pi/32);

    sprintf(histogramName4, "hBackgroundPtBin%d", kPt);
    sprintf(histogramTitle4, "Mixed-event #pi^{0}-h 2D Correlation runction for %5.2f < p_{T} < %5.2f ", NptBins_[kPt], NptBins_[kPt+1]);
    hBackground[histogramName4] = pi0HadCorrFunc.make<TH2D>(histogramName4, histogramTitle4, 32+1,-5.0-0.15,5.0+0.15,32-1,-_pi/2+_pi/32,3*_pi/2-_pi/32);
  }

  hSignal1 = pi0HadCorrFunc.make<TH2D>("hSignal",";#Delta#eta;#Delta#phi",32+1,-5.0-0.15,5.0+0.15,32-1,-_pi/2+_pi/32,3*_pi/2-_pi/32);
  hBackground1 = pi0HadCorrFunc.make<TH2D>("hBackground",";#Delta#eta;#Delta#phi",32+1,-5.0-0.15,5.0+0.15,32-1,-_pi/2+_pi/32,3*_pi/2-_pi/32);

}

// Track quality cuts
bool EcalFlowNtp::TrackQualityCuts(const reco::Track & track, const reco::Vertex & vertexCollectionSelected)
{

  math::XYZPoint vtxPoint(0.0,0.0,0.0);
  double vzErr =0.0, vxErr=0.0, vyErr=0.0;
  vtxPoint=vertexCollectionSelected.position();
  vzErr=vertexCollectionSelected.zError();
  vxErr=vertexCollectionSelected.xError();
  vyErr=vertexCollectionSelected.yError();

  double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
  dxy = track.dxy(vtxPoint);
  dz = track.dz(vtxPoint);
  dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
  dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);
 
  if(track.quality(reco::TrackBase::qualityByName(srcTracks_)) != 1)
    return false;
  if(fabs(dxy/dxysigma) > cutDxyErrMax_) return false;
  if(fabs(dz/dzsigma) > cutDzErrMax_) return false;
  if(track.ptError() / track.pt() > cutPtErrMax_) return false;

  return true;

}

bool EcalFlowNtp::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
  if( a.tracksSize() != b.tracksSize() )
    return  a.tracksSize() > b.tracksSize() ? true : false ;
  else
    return  a.chi2() < b.chi2() ? true : false ;  
}


void EcalFlowNtp::convxtalid(Int_t &nphi,Int_t &neta)
{
  // Barrel only
  // Output nphi 0...359; neta 0...84; nside=+1 (for eta>0), or 0 (for eta<0).
  // neta will be [-85,-1] , or [0,84], the minus sign indicates the z<0 side.
  
  if(neta > 0) neta -= 1;
  if(nphi > 359) nphi=nphi-360;
  
} //end of convxtalid

int EcalFlowNtp::diff_neta_s(Int_t neta1, Int_t neta2){
  Int_t mdiff;
  mdiff=(neta1-neta2);
  return mdiff;
}

// Calculate the distance in xtals taking into account the periodicity of the Barrel
int EcalFlowNtp::diff_nphi_s(Int_t nphi1,Int_t nphi2) {
  Int_t mdiff;
  if(abs(nphi1-nphi2) < (360-abs(nphi1-nphi2))) {
    mdiff=nphi1-nphi2;
  }
  else {
    mdiff=360-abs(nphi1-nphi2);
    if(nphi1>nphi2) mdiff=-mdiff;
  }
  return mdiff;
}


// Method for iterative printing of decay chains
bool EcalFlowNtp::printChildren(const SimTrack* p, 
				  map<const SimTrack*, set<const SimTrack*> > const& ptokids,
				  map<const SimTrack*, const SimVertex*> const& ptovtx,
				  int level, bool save, int motherGenIndex) {

  // Print parent
  bool hasvtx = (ptovtx.find(p) != ptovtx.end());
  
  bool hasKids = (ptokids.find(p) != ptokids.end());

  // Save only SIM tracks not already in hiGenParticle list
  bool saved = false;
  if (save && level > 0 && nSIM < nMaxSIM) {
    pdgIdSIM[nSIM] = p->type();
    statusSIM[nSIM] = (hasKids ? 2 : 1);
    ptSIM[nSIM] = p->momentum().pt();
    eSIM[nSIM] = p->momentum().energy();
    etaSIM[nSIM] = p->momentum().eta();
    phiSIM[nSIM] = p->momentum().phi();
    motherGenIndexSIM[nSIM] = motherGenIndex;
    if (hasvtx) {
      rSIM[nSIM] = ptovtx.find(p)->second->position().Rho();
      zSIM[nSIM] = ptovtx.find(p)->second->position().z();
    }
    else {
      rSIM[nSIM] = zSIM[nSIM] = 0.;
      //assert(hasvtx);
    }
    
    ++nSIM;
    saved = true;
  }

  // Print children, if any
  if (hasKids) {

    set<const SimTrack*> const& kids = ptokids.find(p)->second;
    for (set<const SimTrack*>::const_iterator iKid = kids.begin();
	 iKid != kids.end(); ++iKid)
      saved |= printChildren(*iKid, ptokids, ptovtx, level+1, save, motherGenIndex);
  } // if kids

  return saved;
} // printChildren

float EcalFlowNtp::efficiencyCorrections(double pTtrig, double eta_trg, int tHighPurityTracks_) {
  //This function takes the signal fraction into account for efficiency 
  //corrections

  if((tHighPurityTracks_ >= 120 && tHighPurityTracks_ < 260)) {
    float SignalFraction[8][8] = {{0.067, 0.255, 0.235, 0.334, 0.511, 0.642, 0.723, 0.809},
                               {0.132, 0.280, 0.306, 0.467, 0.579, 0.692, 0.793, 0.826},
                               {0.134, 0.292, 0.341, 0.484, 0.646, 0.760, 0.823, 0.859},
                               {0.162, 0.315, 0.376, 0.503, 0.657, 0.757, 0.867, 0.826},
                               {0.195, 0.328, 0.367, 0.571, 0.678, 0.781, 0.838, 0.856},
			       {0.139, 0.289, 0.340, 0.482, 0.625, 0.709, 0.835, 0.881},
			       {0.123, 0.281, 0.303, 0.442, 0.586, 0.683, 0.765, 0.903},
			       {0.094, 0.260, 0.234, 0.376, 0.485, 0.603, 0.688, 0.678}};
 
    if(eta_trg >= -1.2 && eta_trg < -0.9) {
    if(pTtrig < 1.0)  return SignalFraction[0][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[0][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[0][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[0][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[0][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[0][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[0][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[0][7];
    }

    if(eta_trg >= -0.9 && eta_trg < -0.6) {
    if(pTtrig < 1.0)  return SignalFraction[1][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[1][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[1][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[1][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[1][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[1][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[1][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[1][7];
    }

    if(eta_trg >= -0.6 && eta_trg < -0.3) {
    if(pTtrig < 1.0)  return SignalFraction[2][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[2][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[2][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[2][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[2][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[2][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[2][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[2][7];
    }

    if(eta_trg >= -0.3 && eta_trg < 0.0) {
    if(pTtrig < 1.0)  return SignalFraction[3][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[3][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[3][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[3][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[3][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[3][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[3][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[3][7];
    }

    if(eta_trg >= 0.0 && eta_trg < 0.3) {
    if(pTtrig < 1.0)  return SignalFraction[4][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[4][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[4][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[4][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[4][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[4][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[4][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[4][7];
    }

    if(eta_trg >= 0.3 && eta_trg < 0.6) {
    if(pTtrig < 1.0)  return SignalFraction[5][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[5][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[5][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[5][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[5][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[5][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[5][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[5][7];
    }

    if(eta_trg >= 0.6 && eta_trg < 0.9) {
    if(pTtrig < 1.0)  return SignalFraction[6][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[6][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[6][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[6][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[6][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[6][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[6][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[6][7];
   } 

    if(eta_trg >= 0.9 && eta_trg < 1.2) {
    if(pTtrig < 1.0)  return SignalFraction[7][0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[7][1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[7][2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[7][3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[7][4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[7][5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[7][6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[7][7];
    }

  }

  return 0.6;

  /*  if((tHighPurityTracks_ >= 120 && tHighPurityTracks_ < 150)) {
    float SignalFraction[9] = {0.08, 0.24, 0.29, 0.46, 0.59, 0.68, 0.74, 0.76, 0.77};
    if(pTtrig < 1.0)  return SignalFraction[0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction[1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction[2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction[3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction[4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction[5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction[6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction[7];
    if(pTtrig >= 5.0)  return SignalFraction[8];
  }

  if((tHighPurityTracks_ >= 150 && tHighPurityTracks_ < 185)) {
    float SignalFraction1[9] = {0.08, 0.24, 0.24, 0.41, 0.53, 0.64, 0.71, 0.74, 0.77};
    if(pTtrig < 1.0)  return SignalFraction1[0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction1[1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction1[2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction1[3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction1[4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction1[5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction1[6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction1[7];
    if(pTtrig >= 5.0)  return SignalFraction1[8];
  }

  if((tHighPurityTracks_ >= 185 && tHighPurityTracks_ < 220)) {
    float SignalFraction2[9] = {0.08, 0.24, 0.22, 0.38, 0.51, 0.61, 0.68, 0.72, 0.77};
    if(pTtrig < 1.0)  return SignalFraction2[0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction2[1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction2[2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction2[3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction2[4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction2[5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction2[6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction2[7];
    if(pTtrig >= 5.0)  return SignalFraction2[8];
  }

  if((tHighPurityTracks_ >= 220 && tHighPurityTracks_ < 260)) {
    float SignalFraction3[9] = {0.08, 0.24, 0.19, 0.35, 0.48, 0.59, 0.66, 0.69, 0.77};
    if(pTtrig < 1.0)  return SignalFraction3[0];
    if(pTtrig >= 1.0 && pTtrig<1.5)  return SignalFraction3[1];
    if(pTtrig >= 1.5 && pTtrig<2.0)  return SignalFraction3[2];
    if(pTtrig >= 2.0 && pTtrig<2.5)  return SignalFraction3[3];
    if(pTtrig >= 2.5 && pTtrig<3.0)  return SignalFraction3[4];
    if(pTtrig >= 3.0 && pTtrig<3.5)  return SignalFraction3[5];
    if(pTtrig >= 3.5 && pTtrig<4.0)  return SignalFraction3[6];
    if(pTtrig >= 4.0 && pTtrig<5.0)  return SignalFraction3[7];
    if(pTtrig >= 5.0)  return SignalFraction3[8];
    }*/

}


float EcalFlowNtp::crystalCorrectionFunction(int indexCrystal, float ecalEnergy) {
  // Energy correction factors according to number of crystals in the cluster                                        
  // Clusters with crystal number 1 or 2 have a linear recalibration, i.e. first two numbers in crystalEnergyCorrection array are ignored                                                                                                         
  // Clusters with crystal number from 3 to 9 have a constant recalibration factor given by the crystalEnergyCorredtion array value    
  //                                                                                                        
  //  The recalibration numbers below are based on proccessing 200K MC events on August 1, 2013                       
  //                                                                                                                        

 const float crystalEnergyCorrection[9] = {0.283, 0.154, 0.0728, 0.0549, 0.0677, 0.0626, 0.0533, 0.0413, 0.0214};

 const float singleCrystalIntercept = 0.3415;
 const float singleCrystalSlope = -0.0927;

 const float doubleCrystalIntercept = 0.15573;
 const float doubleCrystalSlope = 0.015088;

 const float secondOrderEnergyCorrection = 1.007;

 if(indexCrystal == 0)
   return secondOrderEnergyCorrection*(1.0 + singleCrystalIntercept + singleCrystalSlope*ecalEnergy);

 if(indexCrystal == 1)
   return secondOrderEnergyCorrection*(1.0 + doubleCrystalIntercept + doubleCrystalSlope*ecalEnergy);

 if(indexCrystal > 1 && indexCrystal < 9)
   return  secondOrderEnergyCorrection*(1.0 + crystalEnergyCorrection[indexCrystal]);

 return 0.0; // default error value                                                                                                   
}

// Go down in chain and remove unwanted decays
bool EcalFlowNtp::pruneKids(const SimTrack* p,
			      map<const SimTrack*, set<const SimTrack*> > & decays,
			      map<const SimTrack*, const SimTrack*> & parent,
			      map<const SimTrack*, const SimVertex*> & vertex,
			      int level) {

  // No children, go one level back
  if (decays.find(p)==decays.end()) return false;
  
  // Prune kids and see if there are any grandchildren left after pruning
  set<const SimTrack*> const& kids = decays.find(p)->second;
  bool hasGrandKids = false;
  bool hasSameType = false;
  unsigned int nPhotons = 0;
  unsigned int nElectrons = 0;
  for (set<const SimTrack*>::const_iterator iKid = kids.begin();
       iKid != kids.end(); ++iKid) {

    bool hasKids = pruneKids(*iKid, decays, parent, vertex, level+1);
    hasGrandKids = hasGrandKids || hasKids;
    hasSameType = hasSameType || (*iKid)->type()==p->type(); 
    if ((*iKid)->type()==22) ++nPhotons;
    if (abs((*iKid)->type())==11) ++nElectrons;
  }
  // if there are grandkids, don't prune kids as we need the whole chain
  if (hasGrandKids) return true;

  // See if we have some reason to prune the kids
  double pt  = p->momentum().pt();
  bool prune = (hasSameType && nPhotons==kids.size()-1) // bremsstrahlung
    || (nElectrons==kids.size() && level!=0) // non-primary photon conversion
    || (abs(p->type())==11 && nPhotons==kids.size()) // weird brem (no e)
    || (abs(p->type())==11 && nPhotons==kids.size()-nElectrons) // ionization
    || (p->type()==111 && pt<0.9 && nPhotons==kids.size()) // low pT pi0      <---important_selection
    || (p->type()==22 && pt<0.9 && nElectrons==kids.size()); // low pT conv
  // || (kids.size()==1); // not a real decay?
  // (NB: electron brems can produce more than one photon)
  // (NG: electrons can turn into a photon with much less pT?)
  // (NB: photon conversions can produce only one electron)
  // (NB: pizeros can produce only one photon with most of pT)
  // (NB: pi+ decays seem to only produce a muon, no neutrinos) 

  // Prune, i.e. remove the parent decay and all the kids from maps
  if (prune) {

    for (set<const SimTrack*>::const_iterator iKid = kids.begin();
	 iKid != kids.end(); ++iKid) {
      parent.erase(*iKid);
      vertex.erase(*iKid);
    } // for kids
    decays.erase(p);

    return false;
  } // if prune
  else // no pruning done
    return true; 

} // pruneKids

//define this as a plug-in
DEFINE_FWK_MODULE(EcalFlowNtp);
