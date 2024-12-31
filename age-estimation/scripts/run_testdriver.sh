#!/bin/bash

source ../common/scripts/utils.sh

# Make sure there aren't any zombie processes
# left over from previous validation run
kill -9 $(ps -aef | grep "count_thread" | awk '{ print $2 }') 2> /dev/null

configDir=config
if [ ! -e "$configDir" ]; then
	echo "${bold}[ERROR] Missing ./$configDir folder!${normal}"
	exit $failure	
fi

outputDir=validation
rm -rf $outputDir; mkdir -p $outputDir

# Usage: ../bin/validate_ae estimateAge|verifyAge -c configDir -o outputDir -h outputStem -i inputFile -t numForks
#
#   estimateAge|verifyAge: task to process
#   configDir: configuration directory
#   outputDir: directory where output logs are written to
#   outputStem: the string to prefix the output filename(s) with
#   inputFile: input file containing images to process (required for enroll and verif template creation)
#   numForks: number of processes to fork

echo "${RED}--------------------------------------- ${END}"
echo "${RED} Running FRVT Age-Estimation Validation ${END}"
echo "${RED}--------------------------------------- ${END}"

echo "${BLUE}checking for hard-coded config directory${END}"
for actionType in estimateAge_1 estimateAge_2 verifyAge
do
    optCmd=""	
    if [[ $actionType == "verifyAge" ]]; then
	action=$actionType
	inputFile=input/short_singleMedia.txt
	optCmd="-a 18"
	echo -n "$action() - "
    else
        action=estimateAge
	optCmd="-x 0"
    	if [[ $actionType == "estimateAge_1" ]]; then
    	    inputFile=input/short_singleMedia.txt
	    echo -n "$action() - "
        elif [[ $actionType == "estimateAge_2" ]];then
    	    inputFile=input/short_twoMedia.txt
	    optCmd="-x 1"
	    echo -n "$action(two inputs) - "
	fi
    fi

    #echo -n "$action ($actionType) - checking for hard-coded config directory "
    
    numForks=1
    outputStem=$action

    tempConfigDir=otherConfig
    chmod 775 $configDir; mv $configDir $tempConfigDir; chmod 550 $tempConfigDir
    bin/validate_ae $action -c $tempConfigDir -o $outputDir -h $outputStem -i $inputFile -t $numForks $optCmd 
    ret=$?
    if [[ $ret == 0 ]]; then
	echo "${GREEN}[SUCCESS]${END}" 
	# Merge output files together
	merge $outputDir/$outputStem log
    elif [[ $ret == 2 ]]; then
	echo "[NOT IMPLEMENTED]"
    else
	chmod 775 $tempConfigDir
	mv $tempConfigDir $configDir
	echo ${RED} [ERROR] Detection of hard-coded config directory in your software.  Please fix! ${END}
	exit $failure
    fi
    rm -rf $outputDir/*
    chmod 775 $tempConfigDir; mv $tempConfigDir $configDir; chmod 550 $configDir
done 

for process in single multiple
do	
    echo "${BLUE}Processing validation in $process process${END}"
    for actionType in estimateAge_1 estimateAge_2 verifyAge
    do
        optCmd=""
        if [[ $actionType == "verifyAge" ]]; then
    	    ageThreshold=18
            action=$actionType
            inputFile=input/singleMedia.txt
            optCmd="-a $ageThreshold"
            echo -n "$action() - "
        else
            action=estimateAge
            optCmd="-x 0"
            if [[ $actionType == "estimateAge_1" ]]; then
                inputFile=input/singleMedia.txt
                echo -n "$action() - "
            elif [[ $actionType == "estimateAge_2" ]];then
                inputFile=input/twoMedia.txt
                optCmd="-x 1"
                echo -n "$action(two inputs) - "
            fi
        fi
    	if [[ $process == "single" ]]; then
            # Start checking for threading
            ../common/scripts/count_threads.sh validate_ae $outputDir/thread.log & pid=$!
    
            outputStem=validation
            bin/validate_ae $action -c $configDir -o $outputDir -h $outputStem -i $inputFile -t $numForks $optCmd
            ret=$?
    
            # End checking for threading
            kill -9 "$pid"
            wait "$pid" 2>/dev/null
    
            if [[ $ret == 0 ]]; then
    	    	echo "${GREEN}[SUCCESS]${END}" 
    	    	# Merge output files together
    	    	merge $outputDir/$outputStem log
    
    	    	maxThreads=$(cat $outputDir/thread.log | sort -u -n | tail -n1)
       	    	# 1 process for testdriver, 1 process for child
    	    	if [ "$maxThreads" -gt "3" ]; then
    	            echo "${bold}[WARNING] We've detected that your software may be threading or using other multiprocessing techniques.  The number of processes detected was $maxThreads and it should be 2.  Per the API document, implementations must run single-threaded.  In the test environment, there is no advantage to threading, because NIST will distribute workload across multiple blades and multiple processes.  We highly recommend that you fix this issue prior to submission.${normal}"
    	        fi
            else
    	    	echo "${bold}[ERROR] $action in age estimation validation (single process) failed${normal}"
    	    	exit $failure
            fi
    
    	    rm -rf $outputDir/*
    	else #$process == "multiple"
            outputStem=$actionType
            numForks=4
            bin/validate_ae $action -c $configDir -o $outputDir -h $outputStem -i $inputFile -t $numForks $optCmd
            ret=$?
            if [[ $ret == 0 ]]; then
    	    	echo "${GREEN}[SUCCESS]${END}"
    	    	# Merge output files together
    	    	merge $outputDir/$outputStem log
            else
    	    	echo "${bold}[ERROR] $action age-estimation validation (multiple process) failed.  Please ensure your software is compatible with fork(2).${normal}"
    	    	exit $failure
            fi
    	fi
    done
done
