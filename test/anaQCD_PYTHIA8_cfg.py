import FWCore.ParameterSet.Config as cms

process = cms.Process('GEN')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic8TeVCollision_cfi')
process.load("PhysicsTools.HepMCCandAlgos.genParticles_cfi")
process.load("RecoJets.Configuration.GenJetParticles_cff")
process.load("RecoJets.Configuration.RecoGenJets_cff")

# ================= var parsing ======================

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('standard')

options.output = 'QCDAna.root'
options.maxEvents = 100

options.register('tune',
                 "4C",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Pythia tune")

options.register('processType',
                 "HardQCDAll",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Pythia process type with pT_hat range")

options.register('sqrtS',
                 2760.0,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.float,
                 "Center-of-mass energy")

options.register('ptHatLow',
                 120,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Minimum pt-hat")

options.register('ptHatHigh',
                 160,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Maximum pt-hat")

options.parseArguments()

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound'),
    makeTriggerResults = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(True)
)

# Other statements
#process.genstepfilter.triggerConditions=cms.vstring("generation_step")

process.MessageLogger.cerr.FwkReport.reportEvery = 100


process.TFileService = cms.Service("TFileService",
    fileName = cms.string(options.output)
)

from IOMC.RandomEngine.RandomServiceHelper import RandomNumberServiceHelper
randSvc = RandomNumberServiceHelper(process.RandomNumberGeneratorService)
randSvc.populate()

process.generator = cms.EDFilter("Pythia8GeneratorFilter",
    pythiaPylistVerbosity = cms.untracked.int32(0),
    filterEfficiency = cms.untracked.double(1),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    comEnergy = cms.double(13000.0),
    #crossSection = cms.untracked.double(175007.8),
    maxEventsToPrint = cms.untracked.int32(0),
    PythiaParameters = cms.PSet(
       processParameters = cms.vstring(),
        parameterSets = cms.vstring('processParameters')
    )
)


# update the process parameters and c.o.m energy
from customisePYTHIA8_cfi import *
updatePy8ProcParameters(process.generator,options.tune,options.processType,options.ptHatLow,options.ptHatHigh,options.sqrtS)

print process.generator.PythiaParameters.processParameters

process.gen_step = cms.Path(process.generator
                            * process.genParticles )

# update the process parameters and c.o.m energy
#from customiseGEN2_cfi import *
#updatePy6ProcParameters(process.generator,options.processType,options.ptHatLow,options.ptHatHigh,options.sqrtS)

#print process.generator.PythiaParameters.processParameters

# ============= Gen jet ================================
process.ak3GenJets = process.ak5GenJets.clone( rParam = 0.3 )
process.ak4GenJets = process.ak5GenJets.clone( rParam = 0.4 )
process.ak7GenJets = process.ak5GenJets.clone( rParam = 0.7 )

process.genjet_step = cms.Path(process.genJetParticles 
                               * process.ak3GenJets
                          #     * process.ak4GenJets
                          #     * process.ak5GenJets
                          #     * process.ak7GenJets
)

# flavor matching

process.myPartons = cms.EDProducer("PartonSelector",
    withLeptons = cms.bool(False),
    src = cms.InputTag("genParticles")
)

process.flavourByRef = cms.EDProducer("JetPartonMatcher",
    jets = cms.InputTag("ak3GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

process.flavor_step = cms.Path( process.myPartons * process.flavourByRef )

# =============== Analysis =============================
process.qcdAna = cms.EDAnalyzer('QCDAnalyzer',
    genJetSrc = cms.InputTag("ak3GenJets"),
    genParticleSrc = cms.InputTag("genParticles"),
    doCMatrix = cms.bool(True),
    doFlavor = cms.bool(True),
    flavorSrc = cms.InputTag("flavourByRef"),
    flavorId = cms.vint32(0),
    jetsByAbsRapidity = cms.bool(False),
    etaMin = cms.double(-1.0),
    etaMax = cms.double(1.0),
    jetRadius = cms.double(0.3),
    pthatMin = cms.double(options.ptHatLow),
    pthatMax = cms.double(options.ptHatHigh),
    ptBins = cms.vdouble( 3, 4, 5, 7, 9, 12, 15, 18, 22, 27, 33, 39, 47, 55, 64, 74, 84, 97, 114, 133, 153, 174, 196, 220, 245, 272, 300, 429, 692, 1000 ),
    pythiaProcess = cms.string(options.processType ),
    dijetEtaBins = cms.vdouble( -3.01, -2.63333, -2.07, -1.78833, -1.50667, -1.225, -0.943333, -0.661667, -0.38, -0.0983333, 0.183333, 0.465, 0.746667, 1.02833, 1.31, 1.59167, 1.87333, 2.43667, 3.0),
    dijetEtaWeights = cms.vdouble( 1, 0.772085, 0.701301, 0.753585, 0.813741, 0.882849, 0.943137, 0.977332, 0.993655, 1.0375, 1.04713, 1.04826, 1.05517, 1.05983, 1.0723, 1.06945, 1.01587, 1.41731 )    
)

process.ana_step = cms.Path(
    process.qcdAna
)

process.schedule = cms.Schedule(process.gen_step,
                                process.genjet_step,
                                process.flavor_step,
                                process.ana_step)

#process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)

# Schedule definition
#process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step)

# filter all path with the production filter sequence
#for path in process.paths:
#	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 

# customisation of the process.

# End of customisation functions
