import FWCore.ParameterSet.Config as cms
muonFilter = cms.EDFilter("HLTHIMuL1L2L3Filter",
    #L2CandTag = cms.InputTag("standAloneMuons"),
    L2CandTag = cms.InputTag("hltL2MuonCandidates"),
    PrimaryVertexTag = cms.InputTag("pixelVertices"),
    NavigationPSet = cms.PSet(
        ComponentName = cms.string('SimpleNavigationSchool')
    ),
    rphiRecHits = cms.InputTag("siStripMatchedRecHits","rphiRecHit"),
    TTRHBuilder = cms.string('WithoutRefit')
)
