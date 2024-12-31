#!/bin/bash

source ../common/scripts/utils.sh

# Check version of OS
check_os

# Install the necessary packages to run validation
check_packages

# Check that required folders exist
check_folders

# Compile and build implementation library against
# validation test driver
scripts/compile_and_link.sh
retcode=$?
if [[ $retcode != 0 ]]; then
	exit $failure
fi
# Set dynamic library path to the folder location of the developer's submission library
export LD_LIBRARY_PATH=$(pwd)/lib

# Run testdriver against linked library
# and validation images
scripts/run_testdriver.sh
retcode=$?
if [[ $retcode != 0 ]]; then
	exit $failure
fi

outputDir="validation"
# Do some sanity checks against the output logs
echo "${BLUE}Sanity checking validation output${END} "

sanityCheck(){
    echo -n "$outputlog - "
    if [ -e "$outputlog" ]; then
        numLogs=$((numLogs+1))
        numLogLines=$(sed '1d' $outputlog | wc -l)
        if [ "$numInputLines" != "$numLogLines" ]; then
            echo "[ERROR] The $outputlog not include results for all of the input images.  Please re-run the validation test."
            exit $failure
        fi

        # Check return codes
        numFail=$(sed '1d' $outputlog | awk '{ if($NF!=0) print }' | wc -l)
        if [ "$numFail" != "0" ]; then
            echo -e "\n${bold}[WARNING] The following entries in $outputlog generated non-successful return codes:${normal}"
            sed '1d' $outputlog | awk '{ if($NF!=0) print }'
        fi
        echo "${GREEN}[DONE]${END}"
    else
	echo "${RED}[MISSING]${END}"
    fi
}

numLogs=0
for actionType in estimateAge_1 estimateAge_2 verifyAge
do
    if [[ $actionType == "verifyAge" ]]; then
        outputlog=validation/$actionType.log
	numInputLines=$(cat input/singleMedia.txt | wc -l)
    else
        if [[ $actionType == "estimateAge_1" ]]; then
            outputlog=validation/$actionType.log
	    numInputLines=$(cat input/singleMedia.txt |wc -l )
        elif [[ $actionType == "estimateAge_2" ]];then
            outputlog=validation/$actionType.log
	    numInputLines=$(cat input/twoMedia.txt | wc -l)
        fi
    fi

    if [ ! -s "$outputlog" ]; then
        continue
    fi
    # Make sure all images in input file have been processed
    sanityCheck 
done


if [ "$numLogs" == "0" ]; then
    echo "${bold}[ERROR] There are no output logs in the validation folder.  Please make sure you have implemented all age estimation functions from the API.${END}"
    exit $failure
elif [ "$numLogs" -lt "3" ]; then
    echo "${bold}[ERROR] At lease 3 validation output logs should be in the validation folder.  Please make sure you have implemented the require age estimation functions from the API.${END}"
    exit $failure
fi

# Create submission archive
echo -n "${BLUE}Creating submission package${END} "
libstring=$(basename `ls ./lib/libfrvt_ae_*_???.so`)
libstring=${libstring%.so}

for directory in config lib validation doc
do
	if [ ! -d "$directory" ]; then
		echo "[ERROR] Could not create submission package.  The $directory directory is missing."
		exit $failure
	fi
done

# write OS to text file
log_os
# append frvt_structs.h version to submission filename
version=$(get_frvt_header_version)

tar -zcf $libstring.v${version}.tar.gz ./config ./lib ./validation ./doc
echo "${GREEN}[DONE]${END}"
echo "
${BLACK}
#################################################################################################################
A submission package has been generated named${END} ${CYAN}$libstring.tar.gz.${END}  

${BLACK}This archive must be properly encrypted and signed before transmission to NIST.
This must be done according to these instructions - ${END}${BLUE}https://www.nist.gov/sites/default/files/nist_encryption.pdf${END}
${BLACK}using the LATEST FRVT Ongoing public key linked from -${END}
${BLUE}https://www.nist.gov/itl/iad/image-group/products-and-services/encrypting-softwaredata-transmission-nist${END}.

${BLACK}For example:
      gpg --default-key <ParticipantEmail> --output <filename>.gpg \\\\
      --encrypt --recipient frvt@nist.gov --sign \\\\
      libfrvt_ae_<company>_<three-digit submission sequence>.tar.gz

Submit the archive to NIST following the instructions at ${END}${BLUE}http://pages.nist.gov/frvt/html/frvt_submission_form.html${END}.${BLACK}
##################################################################################################################
${END}
"
