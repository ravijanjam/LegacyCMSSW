#!/bin/bash
if [ -f goodFileList.txt ]; then
	  rm goodFileList.txt
  fi
  for FILE in `ls $1/tmp_CMSSW_*.stdout`
#  for FILE in `ls $tmp_CMSSW_*.stdout`
  do
	    if grep -q -e '^JobExitCode=0$' $FILE
#	    if grep JobExitCode=0 $FILE
	    	echo "Checking the $FILE"
		then
		LFN=`grep -e '^LFN:' $FILE | cut -d" " -f 2- | sed -e 's/^/\/cms/'`
	    	echo "Testing $LFN"
	        echo $LFN >> goodFileList.txt
	    fi
  done
