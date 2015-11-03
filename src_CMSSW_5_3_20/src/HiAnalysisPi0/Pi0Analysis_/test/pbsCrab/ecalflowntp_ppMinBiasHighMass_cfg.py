
import FWCore.ParameterSet.Config as cms

process = cms.Process("ecalReconstructedPi0")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Geometry.CaloEventSetup.CaloTopology_cfi')
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
process.load('CondCore.DBCommon.CondDBCommon_cfi');
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('HiAnalysisPi0.Pi0Analysis_.ecalReconstructedPi0_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("pp_NoReCal_15February2014V1.root")
                                   )

process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.GlobalTag.globaltag = 'GR_P_V42_AN3::All'

from HeavyIonsAnalysis.Configuration.CommonFunctions_cff import *
overrideCentrality(process)

process.HeavyIonGlobalParameters = cms.PSet(
        centralityVariable = cms.string("HFtowersPlusTrunc"),
        nonDefaultGlauberModel = cms.string(""),
        centralitySrc = cms.InputTag("pACentrality"),
        pPbRunFlip = cms.untracked.uint32(99999999)
        )

process.load('RecoHI.HiCentralityAlgos.HiCentrality_cfi')
process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
#process.load('cmaguire.RpPbAnalysis.PAPileUpVertexFilter_cff')
process.load('RecoHI.HiCentralityAlgos.HiCentrality_cfi')
process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltSingleTrigger = process.hltHighLevel.clone()
process.hltSingleTrigger.HLTPaths = ["HLT_PAZeroBiasPixel_SingleTrack_v1"]

process.ecalReconstructedPi0.AlCaStreamEBpi0Tag = cms.untracked.InputTag("ecalRecHit","EcalRecHitsEB")
process.ecalReconstructedPi0.srcTowers = cms.InputTag("towerMaker")
process.ecalReconstructedPi0.verbose = cms.untracked.bool(True)
process.ecalReconstructedPi0.qualityString_ = cms.untracked.string("highPurity")
process.ecalReconstructedPi0.cutDzErrMax_ = cms.untracked.double(3.0)
process.ecalReconstructedPi0.cutDxyErrMax_ = cms.untracked.double(3.0)
process.ecalReconstructedPi0.cutPtErrMax_ = cms.untracked.double(0.1)
process.ecalReconstructedPi0.vertexZMax = cms.double(15.0)
process.ecalReconstructedPi0.cutMultMin = cms.double(0.0)
process.ecalReconstructedPi0.cutMultMax = cms.double(1000.0)
process.ecalReconstructedPi0.cutMinTrack = cms.double(0.4)

process.ecalReconstructedPi0.doEnergyRecalibration = cms.bool(False);
process.ecalReconstructedPi0.suppressTrackMatchedClusters = cms.bool(True);
process.ecalReconstructedPi0.makeOutputTree = cms.bool(True)
process.ecalReconstructedPi0.clusterStore = cms.bool(True)
process.ecalReconstructedPi0.clusterRecoTrackStore = cms.bool(False)
process.ecalReconstructedPi0.recoTrackStore = cms.bool(False)

process.ecalReconstructedPi0.doSwissThreshold = cms.bool(False)
process.ecalReconstructedPi0.swissThreshold = cms.double(0.95)

process.ecalReconstructedPi0.doOneDimensionTimeCut = cms.bool(False)
process.ecalReconstructedPi0.oneDimensionTimeCut = cms.double(10.0)

process.ecalReconstructedPi0.doTwoDimensionTimeCut = cms.bool(False)
process.ecalReconstructedPi0.twoDimensionTimeCutFactor = cms.double(0.35)

process.ecalReconstructedPi0.etaBins = cms.vdouble(
            -1.46, -1.36, -1.26, -1.16, -1.06, -0.96, -0.86, -0.76, -0.66, -0.56,
                    -0.46, -0.36, -0.26, -0.16, -0.06, 0.04, 0.14, 0.24, 0.34, 0.44, 0.54,
                    0.64, 0.74, 0.94, 0.94, 1.04, 1.14, 1.24, 1.34, 1.44)

process.ecalReconstructedPi0.occBins = cms.vdouble(
            0., 20., 40., 60., 80., 100., 120., 140., 160.,
                        180., 200., 250., 300., 500.
			)
process.ecalReconstructedPi0.occByCentrality = cms.bool(True)
process.ecalReconstructedPi0.diHadronCorrelations = cms.bool(False)
                        
process.ecalReconstructedPi0.pi0HadronCorrelations = cms.bool(True)
process.ecalReconstructedPi0.etaHadronCorrelations = cms.bool(False)
process.ecalReconstructedPi0.doEffCorrections = cms.bool(True);
process.ecalReconstructedPi0.lowpi0PtCut = cms.double(0.7)
process.ecalReconstructedPi0.highpi0PtCut = cms.double(12.0)
process.ecalReconstructedPi0.pfCandidatesTag = cms.InputTag("particleFlow")
process.ecalReconstructedPi0.cutByLeadingTrackPt = cms.bool(False)
process.ecalReconstructedPi0.leadingTrackPtMin = cms.double(0.0)
process.ecalReconstructedPi0.leadingTrackPtMax = cms.double(999.0)
process.ecalReconstructedPi0.cutByLeadingPhotonPt = cms.bool(False)
process.ecalReconstructedPi0.leadingPhotonPtMin = cms.double(0.0)
process.ecalReconstructedPi0.leadingPhotonPtMax = cms.double(999.0)
process.ecalReconstructedPi0.timeThreshold = cms.double(10.0)
process.ecalReconstructedPi0.avoidIeta85 = cms.double(False)
process.ecalReconstructedPi0.useClusterCrystalLimitLowPtOnly = cms.bool(False);
process.ecalReconstructedPi0.clusterCrystalLowPtOnly = cms.double(1.5)
process.ecalReconstructedPi0.doWeightedTimeCut = cms.bool(False)

process.ecalReconstructedPi0.ptBins = cms.vdouble(1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 3.0, 3.5, 4.0, 4.5, 5.0,
                                         5.5, 6.0, 7.0, 8.0, 10.0, 12.0
                                        )
process.ecalReconstructedPi0.NptBins = cms.vdouble(1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0)

process.path = cms.Path(process.hltSingleTrigger *
                        process.PAcollisionEventSelection *
                        process.siPixelRecHits *
#                        process.pileupVertexFilterCutGplus *
#                        process.pACentrality *
                        process.ecalReconstructedPi0 
                        )

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
#                                        '/store/hidata/HIRun2013/PAMinBiasUPC/RECO/28Sep2013-v1/10000/001397FC-462D-E311-A034-782BCB3BCADD.root'
					 '/store/data/Run2013A/PPMinBias/RECO/PromptReco-v1/000/211/752/00000/00C017DB-A176-E211-926D-002481E0D7D8.root'
                                                              )
                            )
