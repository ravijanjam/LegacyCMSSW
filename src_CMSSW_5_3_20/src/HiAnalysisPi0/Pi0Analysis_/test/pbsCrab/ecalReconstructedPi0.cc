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

#include "HiAnalysisPi0/Pi0Analysis_/interface/pi0EcalReconstruction.h"

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

pi0EcalReconstruction::pi0EcalReconstruction(const edm::ParameterSet& ps) 
{
	cout << "Inside ParameterSet method" << endl;
	testMessage = ps.getUntrackedParameter<edm::InputTag>("testMessage");
}

pi0EcalReconstruction::~pi0EcalReconstruction()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
}

// ------------ method called to for each event  ------------
void
pi0EcalReconstruction::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	int c = 0; 
	cout << c++ << " " ;
}

 
// Testing - Ravi - BEGIn
void myClass::beginJob(){}


// Testing - Ravi - End

// ------------ method called once each job just before starting event loop  ------------
void pi0EcalReconstruction::beginJob() 
{
		cout << "from beginJob" << endl;
}


// ------------ method called once each job just after ending the event loop  ------------
void pi0EcalReconstruction::endJob() {}

void pi0EcalReconstruction::initHistos(const edm::Service<TFileService> & fs){}

// Track quality cuts
bool pi0EcalReconstruction::TrackQualityCuts(const reco::Track & track, const reco::Vertex & vertexCollectionSelected)
{cout << "Inside Track Quality cuts method" << endl;return 1;}

bool pi0EcalReconstruction::vtxSort( const reco::Vertex &  a, const reco::Vertex & b ){return 1;}

void pi0EcalReconstruction::convxtalid(Int_t &nphi,Int_t &neta) {}

int pi0EcalReconstruction::diff_neta_s(Int_t neta1, Int_t neta2){return 1;}

int pi0EcalReconstruction::diff_nphi_s(Int_t nphi1,Int_t nphi2){return 1;}

bool pi0EcalReconstruction::printChildren(const SimTrack* p, 
				  map<const SimTrack*, set<const SimTrack*> > const& ptokids,
				  map<const SimTrack*, const SimVertex*> const& ptovtx,
				  int level, bool save, int motherGenIndex) {return 1;} // printChildren

float pi0EcalReconstruction::efficiencyCorrections(double pTtrig, double eta_trg, int tHighPurityTracks_) {return 1;}


float pi0EcalReconstruction::crystalCorrectionFunction(int indexCrystal, float ecalEnergy){return 1;}

// Go down in chain and remove unwanted decays
bool pi0EcalReconstruction::pruneKids(const SimTrack* p,
			      map<const SimTrack*, set<const SimTrack*> > & decays,
			      map<const SimTrack*, const SimTrack*> & parent,
			      map<const SimTrack*, const SimVertex*> & vertex,
			      int level) {return 1;} // pruneKids

//define this as a plug-in
DEFINE_FWK_MODULE(pi0EcalReconstruction);
