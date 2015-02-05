import FWCore.ParameterSet.Config as cms

def getPy8ProcParameters(tune,genType):
    basic_para = [ 'Main:timesAllowErrors    = 10000', 
                   'ParticleDecays:limitTau0 = on',
                   'ParticleDecays:tauMax = 10' ]
    tune_para = { '4C':['Tune:pp 8','Tune:ee 3'] }
    process_para = {
        'inelastic':['SoftQCD:inelastic = on'], 
        'NSD':['SoftQCD:doubleDiffractive = on','SoftQCD:nonDiffractive = on'],
        'HardQCDAll':['HardQCD:all = on']
        }
    para = basic_para + tune_para[tune] + process_para[genType]
    print "PYTHIA process parameters: ",para
    return para

def getPy8PthatLow(pthatLow):
    print "Minimum pt-hat: "+str(pthatLow)
    return ['PhaseSpace:pTHatMin='+str(pthatLow)] 

def getPy8PthatHigh(pthatHigh):
    print "Maximum pt-hat: "+str(pthatHigh)
    return ['PhaseSpace:pTHatMax='+str(pthatHigh)]


def updatePy8ProcParameters(gen,tune,genType,low,high,sqrtS):
    print "Center-of-mass energy: ",sqrtS
    gen.PythiaParameters.processParameters = getPy8ProcParameters(tune,genType)
    gen.PythiaParameters.processParameters += getPy8PthatLow(low)
    gen.PythiaParameters.processParameters += getPy8PthatHigh(high)
    gen.comEnergy = sqrtS

    


