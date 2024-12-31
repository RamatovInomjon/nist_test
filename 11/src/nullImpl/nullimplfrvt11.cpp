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
#include <iomanip>

#include "nullimplfrvt11.h"

using namespace std;
using namespace FRVT;
using namespace FRVT_11;

NullImplFRVT11::NullImplFRVT11() {}

NullImplFRVT11::~NullImplFRVT11() {}

ReturnStatus
NullImplFRVT11::initialize(const std::string &configDir)
{
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVT11::createFaceTemplate(
        const std::vector<FRVT::Image> &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates)
{
    /* Note: example code, potentially not portable across machines. */
    std::vector<float> fv = {0.388819890, 2.0, 8.88, 765.88989};
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(fv.data());
    int dataSize = sizeof(float) * fv.size();
    templ.resize(dataSize);
    memcpy(templ.data(), bytes, dataSize);

    for (unsigned int i = 0; i < faces.size(); i++) {
        eyeCoordinates.push_back(EyePair(true, true, i, i, i+1, i+1));
    }

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVT11::createIrisTemplate(
        const std::vector<FRVT::Image> &irises,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<IrisAnnulus> &irisLocations)
{
    return ReturnStatus(ReturnCode::NotImplemented);
}

ReturnStatus
NullImplFRVT11::createFaceTemplate(
    const FRVT::Image &image,
    FRVT::TemplateRole role,
    std::vector<std::vector<uint8_t>> &templs,
    std::vector<FRVT::EyePair> &eyeCoordinates)
{
    int numFaces = rand() % 4 + 1;
    for (int i = 1; i <= numFaces; i++) {
        std::vector<uint8_t> templ;
        /* Note: example code, potentially not portable across machines. */
        std::vector<float> fv = {1.0, 2.0, 8.88, 765.88989};
        /* Multiply vector values by scalar */
        for_each(fv.begin(), fv.end(), [i](float &f){ f *= i; });
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(fv.data());
        int dataSize = sizeof(float) * fv.size();
        templ.resize(dataSize);
        memcpy(templ.data(), bytes, dataSize);
        templs.push_back(templ);

        eyeCoordinates.push_back(EyePair(true, true, i, i, i+1, i+1));
    } 
    
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVT11::matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &enrollTemplate,
        double &score)
{
    float *featureVector = (float *)enrollTemplate.data();

    for (unsigned int i=0; i<this->featureVectorSize; i++) {
	std::cout << std::setprecision(10) << featureVector[i] << std::endl;
    }

    score = rand() % 1000 + 1;
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVT11>();
}





