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
stem="validation"

# Do some sanity checks against the output logs
echo -n "Sanity checking validation output "
for input in enroll_1N search_1N
do
	numInputLines=$(cat input/$input.txt | wc -l)
	numLogLines=$(cat $outputDir/$stem.$input | wc -l)
	if [ "$input" == "enroll_1N" ]; then
		# Number of stills/frames + header
        numExpectedLines=$(grep -o "unknown" input/$input.txt | wc -l)
		numExpectedLines=$(($numExpectedLines + 1))

        # Check the output logs have the right number of lines
        if [ "$numLogLines" -ne "$numExpectedLines" ]; then
            echo "[ERROR] The $outputDir/$stem.$input file has the wrong number of lines.  It should contain $numExpectedLines but contains $numLogLines.  Please re-run the validation test."
            exit $failure
        fi	
	fi

	# Check template generation and search return codes
	if [ "$input" == "enroll_1N" ]; then
		numFail=$(sed '1d' $outputDir/$stem.$input | awk '{ if($4!=0) print }' | wc -l)
		if [ "$numFail" -ne "0" ]; then
            echo "[WARNING] The following enrollment entries generated non-successful return codes."
			sed '1d' $outputDir/$stem.$input | awk '{ if($4!=0) print }'
		fi
	elif [ "$input" == "search_1N" ]; then
		numFail=$(sed '1d' $outputDir/$stem.$input | awk '{ if($3!=0) print }' | wc -l)
		if [ "$numFail" -ne "0" ]; then
            echo "[WARNING] The following search entries generated non-successful return codes."
			sed '1d' $outputDir/$stem.$input | awk '{ if($3!=0) print }'
		fi
	fi
done
echo "[SUCCESS]"

# Create submission archive
echo -n "Creating submission package "
libstring=$(basename `ls ./lib/libfrte_five_*_???.so`)
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
# append five_structs.h version to submission filename
major=$(grep "FIVE_STRUCTS_MAJOR_VERSION{" src/include/five_structs.h | awk -F'{' '{ print $2 }' | awk -F'}' '{ print $1 }')
minor=$(grep "FIVE_STRUCTS_MINOR_VERSION{" src/include/five_structs.h | awk -F'{' '{ print $2 }' | awk -F'}' '{ print $1 }')

version="${major}.${minor}"
libstring="$libstring.v${version}"
tar -zcf $libstring.tar.gz ./doc ./config ./lib ./validation
echo "[SUCCESS]"
echo "
#################################################################################################################
A submission package has been generated named $libstring.tar.gz.  DO NOT RENAME THIS FILE. 

This archive must be properly encrypted and signed before transmission to NIST.
This must be done according to these instructions - https://www.nist.gov/sites/default/files/nist_encryption.pdf
using the FRTE/FATE public key linked from -
https://www.nist.gov/itl/iad/image-group/products-and-services/encrypting-softwaredata-transmission-nist.

For example:
      gpg --default-key <ParticipantEmail> --output <filename>.gpg \\\\
      --encrypt --recipient frvt@nist.gov --sign \\\\
      libfrte_five_<company>_<three-digit submission sequence>.v<validation_package_version>.tar.gz

Submit the encrypted file and your public key to NIST following the instructions at http://pages.nist.gov/frvt/html/frvt_submission_form.html.
##################################################################################################################
"
