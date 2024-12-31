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
echo -n "Sanity checking validation output "
# Check to make sure the validation output folder isn't empty
if [ -z "$(ls -A $outputDir)" ]; then
	echo "[ERROR] The validation output directory is empty.  Developers must implement AT LEAST one function specified from the API."
	exit
fi

for input in detectNonScannedMorph detectScannedMorph detectUnknownMorph detectNonScannedMorphWithProbeImg detectScannedMorphWithProbeImg detectUnknownMorphWithProbeImg compare demorph demorphDifferentially
do
	numInputLines=$(cat input/$input.txt | wc -l)
	if [ ! -s "$outputDir/$input.log" ]; then
		continue
	fi
	numLogLines=$(sed '1d' $outputDir/$input.log | wc -l)
	# account for output log header
	if [ $numInputLines != "$numLogLines" ]; then
		echo "[ERROR] The $outputDir/$input.log file has the wrong number of lines.  It should contain $((numInputLines+1)) but contains $numLogLines.  Please re-run the validation test."
		exit $failure
	fi
done
echo "[SUCCESS]"

# Create submission archive
echo -n "Creating submission package "
libstring=$(basename `ls ./lib/libfrvt_morph_*_???.so`)
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

out_lib_name="${libstring}.v${version}.tar.gz"
tar -zcf "${out_lib_name}" ./config ./lib ./validation ./doc
echo "[SUCCESS]"
echo "
#################################################################################################################
A submission package has been generated named '${out_lib_name}'. DO NOT RENAME THIS FILE. 

This archive must be properly encrypted and signed before transmission to NIST.
This must be done according to these instructions - https://www.nist.gov/sites/default/files/nist_encryption.pdf
using the LATEST FRVT Ongoing public key linked from -
https://www.nist.gov/itl/iad/image-group/products-and-services/encrypting-softwaredata-transmission-nist.

For example:
      gpg --default-key <ParticipantEmail> --output <filename>.gpg \\\\
      --encrypt --recipient frvt@nist.gov --sign \\\\
      libfrvt_morph_<company>_<three-digit submission sequence>.v<validation_package_version>.tar.gz

Submit the encrypted file and your public key to NIST following the instructions at http://pages.nist.gov/frvt/html/frvt_submission_form.html.
##################################################################################################################
"
