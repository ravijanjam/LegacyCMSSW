# GenOnTheFly
CMS EDAnalyzers, python configurations, and submission tools for performing gen-level studies "on the fly" by analyzing events as they are generated and then discarding them

To run a small 100 event test:

```
scram p CMSSW CMSSW_7_3_1
cd CMSSW_7_3_1/src
cmsenv

mkdir Appeltel
cd Appeltel
git clone https://github.com/appeltel/GenOnTheFly.git
cd GenOnTheFly
scram b
cd test 
cmsRun anaQCD_PYTHIA8_cfg.py
```
