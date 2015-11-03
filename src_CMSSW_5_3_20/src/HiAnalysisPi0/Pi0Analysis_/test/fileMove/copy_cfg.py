import FWCore.ParameterSet.Config as cms

process = cms.Process('COPYT2VANDERBILT')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('/store/user/vzhukova/HIJING_MB/HIJING_MB_RECO_v3/13a591fee6315e7fb1e99e6ba8e52eaa/reco_hijing_1000_1_Bov.root')
#    fileNames = cms.untracked.vstring('/store/hidata/HIRun2013/PAHighPt/RECO/PromptReco-v1/000/211/631/00000/FEDE0B60-3F75-E211-8FE3-003048D2BC5C.root')
)

process.options = cms.untracked.PSet(
)


# Output definition

process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('file:output.root')
)


# Other statements

# Path and EndPath definitions
process.output_step = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.output_step)

