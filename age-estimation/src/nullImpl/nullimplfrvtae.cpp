/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <random>

#include "nullimplfrvtae.h"

using namespace std;
using namespace FRVT;
using namespace FRVT_AE;

NullImplFRVTAE::NullImplFRVTAE() {}

NullImplFRVTAE::~NullImplFRVTAE() {}

ReturnStatus
NullImplFRVTAE::initialize(const std::string &configDir)
{
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTAE::estimateAge(
        const FRVT::Media &face,
        double &age)
{
    age = rand() % 100 + 1;
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTAE::estimateAge(
	const FRVT::Media &faceOne,
        const double &ageOne,
        const FRVT::Media &faceTwo,
        double &age)
{
    age = rand() % 100 + 1;
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTAE::verifyAge(
        const FRVT::Media &face,
        const double &age,
	bool &isAboveThreshold)
{
    isAboveThreshold = false;
    if ((1 + ( rand() % 50 )) % 2 == 1){
	isAboveThreshold = true;
    }
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVTAE>();
}





