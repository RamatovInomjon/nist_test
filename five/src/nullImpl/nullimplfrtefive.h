/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef NULLIMPLFRTEFIVE_H_
#define NULLIMPLFRTEFIVE_H_

#include <map>
#include "frte_five.h"

/*
 * Declare the implementation class of the FRTE FIVE Interface
 */
namespace FIVE {
    class NullImplFRTEFIVE : public FIVE::Interface {
public:

    NullImplFRTEFIVE();
    ~NullImplFRTEFIVE() override;

    FIVE::ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir) override;

    FIVE::ReturnStatus
    createEnrollmentTemplate(
        const std::vector<FIVE::Media> &media,
        std::vector<uint8_t> &personTemplate,
        std::vector< std::vector<FIVE::BoundingBox> > &personTracks) override;

    FIVE::ReturnStatus
    createSearchTemplate(
        const FIVE::Media &media,
        std::vector< std::vector<uint8_t> > &personTemplates,
        std::vector< std::vector<FIVE::BoundingBox> > &personTracks) override;

    FIVE::ReturnStatus
    finalizeEnrollment(
        const std::string &configDir,
        const std::string &enrollmentDir,
        const std::string &edbName,
        const std::string &edbManifestName,
        FIVE::GalleryType galleryType) override;

    FIVE::ReturnStatus
    initializeSearch(
        const std::string &configDir,
        const std::string &enrollmentDir) override;

    FIVE::ReturnStatus
    search(
        const std::vector<uint8_t> &searchTemplate,
        const uint32_t &candidateListLength,
        std::vector<FIVE::Candidate> &candidateList) override;

    FIVE::ReturnStatus
    getThreshold(
        const uint32_t &numPeopleInGallery,
        const double &intendedFPIR,
        double &threshold) override;

    static std::shared_ptr<FIVE::Interface>
    getImplementation();

private:
    std::map<std::string, std::vector<uint8_t>> templates;

    const std::string edb{"mei.edb"};
    const std::string manifest{"mei.manifest"}; 
    
    int counter;
};
}

#endif /* NULLIMPLFRTEFIVE_H_ */
