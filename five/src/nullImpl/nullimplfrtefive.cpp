/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "nullimplfrtefive.h"

using namespace std;
using namespace FIVE;

NullImplFRTEFIVE::NullImplFRTEFIVE() {}

NullImplFRTEFIVE::~NullImplFRTEFIVE() {}

ReturnStatus
NullImplFRTEFIVE::initializeTemplateCreation(
    const std::string &configDir)
{
    // Load some stuff from the configuration directory, etc...
    this->counter = 0;
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRTEFIVE::createEnrollmentTemplate(
    const std::vector<FIVE::Media> &media,
    std::vector<uint8_t> &personTemplate,
    std::vector< std::vector<FIVE::BoundingBox> > &personTracks)
{
    auto templString = std::to_string(media.size()) +
            " Somewhere out there (five enrollment template), beneath the pale moon light\n";
    personTemplate.resize(templString.size());
    memcpy(personTemplate.data(), templString.c_str(), templString.size());

    // Populate bounding boxes
    // for (const auto& oneMedia : media) {
    //    std::vector<FIVE::BoundingBox> bbs(oneMedia.data.size(), BoundingBox(1,2,3,4));
    //    personTracks.push_back(bbs);
    // } 
    
    return ReturnStatus(ReturnCode::Success);
}

void
writeImage(
    const uint8_t* decompressed_data,
    const int size,
    const int width,
    const int height,
    const std::string &outputStem)
{
    FILE *fp = fopen((outputStem + ".ppm").c_str(), "wb");
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    fwrite(decompressed_data, 1, size, fp);
    fclose(fp);
}

ReturnStatus
NullImplFRTEFIVE::createSearchTemplate(
    const FIVE::Media &media,
    std::vector< std::vector<uint8_t> > &personTemplates,
    std::vector< std::vector<FIVE::BoundingBox> > &personTracks)
{
    /*
    for (int i = 0; i < media.data.size(); i++) {
        std::ostringstream str;
        str << std::setw(4) << std::setfill('0') << i;
        writeImage(
            media.data[i].data.get(),
            media.data[i].size(),
            media.data[i].width,
            media.data[i].height,
            "validation/" + str.str());
    } 
    */

    std::string templString = std::string("Somewhere out there (five search template), beneath the pale moon light\n");
    std::vector<uint8_t> templ;
    templ.resize(templString.size());
    memcpy(templ.data(), templString.c_str(), templString.size());

    for (int i = 0; i <= (this->counter++ % 2); i++)
        personTemplates.push_back(templ);

    // Populate bounding boxes - there could be one or more people
    // in single piece of probe media
    // for (const auto& onePerson : personTemplates) {
    //    std::vector<FIVE::BoundingBox> bbs(media.data.size());
    //    personTracks.push_back(bbs);
    // }

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRTEFIVE::finalizeEnrollment(
    const std::string &configDir,
    const std::string &enrollmentDir,
    const std::string &edbName,
    const std::string &edbManifestName,
    FIVE::GalleryType galleryType)
{
    ifstream edbsrc(edbName, ios::binary);
    ofstream edbdest(enrollmentDir+"/"+this->edb, ios::binary);
    ifstream manifestsrc(edbManifestName, ios::binary);
    ofstream manifestdest(enrollmentDir+"/"+this->manifest, ios::binary);

    edbdest << edbsrc.rdbuf();
    manifestdest << manifestsrc.rdbuf();

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRTEFIVE::initializeSearch(
    const std::string &configDir,
    const std::string &enrollmentDir)
{
    auto edbManifestName = enrollmentDir + "/" + this->manifest;
    auto edbName = enrollmentDir + "/" + this->edb;

    ifstream manifestStream(edbManifestName.c_str());
    if (!manifestStream.is_open()) {
        cerr << "Failed to open stream for " << edbManifestName << "." << endl;
        return ReturnStatus(ReturnCode::ConfigError);
    }

    ifstream edbStream(edbName, ios::in | ios::binary);
    if (!edbStream.is_open()) {
        cerr << "Failed to open stream for " << edbName << "." << endl;
        return ReturnStatus(ReturnCode::ConfigError);
    }

    string templId, size, offset;
    while (manifestStream >> templId >> size >> offset) {
        edbStream.seekg(atol(offset.c_str()), ios::beg);
        std::vector<uint8_t> templData(atol(size.c_str()));
        edbStream.read((char*) &templData[0], atol(size.c_str()));

        // Insert template + id into map
        this->templates.insert(std::make_pair(templId, templData));
    }

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRTEFIVE::search(
    const std::vector<uint8_t> &searchTemplate,
    const uint32_t &candidateListLength,
    std::vector<FIVE::Candidate> &candidateList)
{
    std::vector<std::string> templateIds;
    for (auto const& element : this->templates)
        templateIds.push_back(element.first);

    // initializeSearch() may not have been called? 
    if (templateIds.size() == 0) {
        return ReturnStatus(ReturnCode::VendorError, "Enrollment template vector is empty!");
    }

    double score = 0.9899; 
    for (unsigned int i = 0; i < candidateListLength; i++) {
        candidateList.push_back(Candidate(true, templateIds[i%(templateIds.size())], score));
        score = score/2;
    }

    return ReturnStatus(ReturnCode::Success);
}

FIVE::ReturnStatus
NullImplFRTEFIVE::getThreshold(
    const uint32_t &numPeopleInGallery,
    const double &intendedFPIR,
    double &threshold)
{
    threshold = 55.0;

    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return make_shared<NullImplFRTEFIVE>();
}
