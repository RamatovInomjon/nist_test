/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <cstring>
#include <iostream>
#include "nullimplfrvtmorph.h"

using namespace std;
using namespace FRVT;
using namespace FRVT_MORPH;

NullImplFRVTMorph::NullImplFRVTMorph() {}

NullImplFRVTMorph::~NullImplFRVTMorph() {}

ReturnStatus
NullImplFRVTMorph::initialize(
    const std::string &configDir,
    const std::string &configValue)
{
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTMorph::detectMorph(
    const FRVT::Image &suspectedMorph,
    const FRVT_MORPH::ImageLabel &label,
    bool &isMorph,
    double &score)
{
    if (label==FRVT_MORPH::ImageLabel::Scanned)
        return ReturnStatus(ReturnCode::NotImplemented);

    isMorph = true;
    score = 0.99;
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTMorph::detectMorphDifferentially(
    const FRVT::Image &suspectedMorph,
    const FRVT_MORPH::ImageLabel &label,
    const FRVT::Image &liveFace,
    bool &isMorph,
    double &score)
{
    if (label==FRVT_MORPH::ImageLabel::Unknown)
        return ReturnStatus(ReturnCode::NotImplemented);

    if (label==FRVT_MORPH::ImageLabel::NonScanned) {
        isMorph = false;
        score = 0.003;
    } else if (label==FRVT_MORPH::ImageLabel::Scanned)
        isMorph = true;
        score = 0.81;
    return ReturnStatus(ReturnCode::Success);
}

FRVT::ReturnStatus
NullImplFRVTMorph::detectMorphDifferentially(
    const FRVT::Image &suspectedMorph,
    const FRVT_MORPH::ImageLabel &label,
    const FRVT::Image &liveFace,
    const FRVT_MORPH::SubjectMetadata &subjectMetadata,
    bool &isMorph,
    double &score)
{
   if (label==FRVT_MORPH::ImageLabel::Unknown)
        return ReturnStatus(ReturnCode::NotImplemented);
    
    isMorph = false;
    score = 0.124;
    return ReturnStatus(ReturnCode::Success); 
}

ReturnStatus
NullImplFRVTMorph::compareImages(
    const Image &enrollImage,
    const Image &verifImage,
    double &similarity)
{
    similarity = 0.88;
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTMorph::demorph(
    const FRVT::Image &suspectedMorph,
    FRVT::Image &outputSubject1,
    FRVT::Image &outputSubject2,
    bool &isMorph,
    double &score)
{
    uint8_t *data = new uint8_t[suspectedMorph.size()];
    std::memcpy(data, suspectedMorph.data.get(), suspectedMorph.size());
    outputSubject1.data.reset(data, std::default_delete<uint8_t[]>());
    outputSubject1.width = suspectedMorph.width;
    outputSubject1.height = suspectedMorph.height;
    outputSubject1.depth = suspectedMorph.depth;
    
    uint8_t *data2 = new uint8_t[suspectedMorph.size()];
    std::memcpy(data2, suspectedMorph.data.get(), suspectedMorph.size());
    outputSubject2.data.reset(data2, std::default_delete<uint8_t[]>());
    outputSubject2.width = suspectedMorph.width;
    outputSubject2.height = suspectedMorph.height;
    outputSubject2.depth = suspectedMorph.depth;

    return ReturnStatus(ReturnCode::NotImplemented);     
}

ReturnStatus
NullImplFRVTMorph::demorphDifferentially(
    const FRVT::Image &suspectedMorph,
    const FRVT::Image &probeFace,
    FRVT::Image &outputSubject,
    bool &isMorph,
    double &score)
{
    uint8_t *data = new uint8_t[suspectedMorph.size()];
    std::memcpy(data, suspectedMorph.data.get(), suspectedMorph.size());
    outputSubject.data.reset(data, std::default_delete<uint8_t[]>());
    outputSubject.width = suspectedMorph.width;
    outputSubject.height = suspectedMorph.height;
    outputSubject.depth = suspectedMorph.depth;

    return ReturnStatus(ReturnCode::NotImplemented);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVTMorph>();
}

