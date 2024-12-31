/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRTE_FIVE_H_
#define FRTE_FIVE_H_

#include <cstdint>
#include <string>
#include <vector>

#include <five_structs.h>

namespace FIVE {

/**
 * @brief
 * The interface to the FIVE implementation
 *
 * @details
 * The submission software under test will implement this interface by
 * sub-classing this class and implementing each method therein.
 */
class Interface {
public:
    virtual ~Interface() {}

    /**
     * @brief Before imagery is sent to the template
     * creation function(s), the test harness will call this initialization
     * function.
     * @details This function will be called N=1 times by the NIST application,
     * prior to parallelizing M >= 1 calls to createTemplate() via fork().
     *
     * This function will be called from a single process/thread.
     *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
     * parameters or run-time data files.
     */
    virtual FIVE::ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir) = 0;

    /**
     * @brief This function supports enrollment template generation from one or more pieces
     * of media of the same person.  It takes a vector of input media and outputs a single 
     * template.
     *
     * @details
     * - The input media vector can contain any combination of one or more 
     *   videos or stills for a single subject.
     * - Media only contains a single person
     *
     * If the function executes correctly (i.e. returns a successful exit status),
     * the template(s) will be enrolled into a gallery.  The NIST
     * calling application may store the resulting template(s),
     * concatenate many templates, and pass the result to the enrollment
     * finalization function.  When the implementation fails to produce a
     * template, it shall still return a blank template (which can be zero
     * bytes in length). The template will be included in the
     * enrollment database/manifest like all other enrollment templates, but
     * is not expected to contain any feature information.
     * 
     * param[in] media
     * A vector of Media objects
     * param[out] personTemplate
     * A single output template.  The format of the template is entirely
     * unregulated.  
     * param[out] personTracks (optional)
     * For each still/frame in each piece of media, the function can optionally return
     * bounding boxes around the detected face for the subject. 
     * This will be an empty vector when passed into the
     * function, and the implementation shall populate it with the appropriate
     * number of entries.  If the developer chooses to return bounding boxes, values in 
     * personTracks[i] should correspond to input media[i].
     */
    virtual FIVE::ReturnStatus
    createEnrollmentTemplate(
        const std::vector<FIVE::Media> &media,
        std::vector<uint8_t> &personTemplate,
        std::vector< std::vector<FIVE::BoundingBox> > &personTracks) = 0;

    /**
     * @brief This function supports search/probe/identification template generation of one or more people detected
     * in the provided input media.  It takes a single piece of input media (still photo or video frames) 
     * and outputs one or more templates and optionally, tracks based on the number of people detected.
     *
     * @details
     * If the function returns a non-successful return status, the output template(s) will be not be used
     * in subsequent search operations.
     *
     * param[in] media
     * A single piece of media (still photo or video frames) that contains one or more people in the photo
     * param[out] personTemplates 
     * A vector of output template(s).  The format of the template(s) is entirely
     * unregulated.  This will be an empty vector when passed into the function, and
     * the implementation can resize and populate it with the appropriate data.
     * param[out] personTracks (optional)
     * For each person detected in the media, the function can optionally return
     * bounding boxes around the detected face.
     * This will be an empty vector when passed into the
     * function, and the implementation should populate it with the appropriate
     * number of entries.  If the developer chooses to return bounding boxes, values in 
     * personTracks[i] shall correspond to the person in personTemplates[i].  
     */
    virtual FIVE::ReturnStatus
    createSearchTemplate(
        const FIVE::Media &media,
        std::vector< std::vector<uint8_t> > &personTemplates,
        std::vector< std::vector<FIVE::BoundingBox> > &personTracks) = 0;

     /**
      * @brief This function will be called after all enrollment templates have
      * been created and freezes the enrollment data.
      * After this call, the enrollment dataset will be forever read-only.
      *
      * @details This function allows the implementation to conduct,
      * for example, statistical processing of the feature data, indexing and
      * data re-organization.  The function may create its own data structure.
      * It may increase or decrease the size of the stored data.  No output is
      * expected from this function, except a return code.  The function will
      * generally be called in a separate process after all the enrollment processes
      * are complete.
      * NOTE: Implementations shall not move the input data.  Implementations
      * shall not point to the input data.
      * Implementations should not assume the input data would be readable
      * after the call.  Implementations must,
      * <b>at a minimum, copy the input data</b> or otherwise extract what is
      * needed for search.
      *
      * This function will be called from a single process/thread.
      *
      * @param[in] configDir
      * A read-only directory containing any developer-supplied configuration
      * parameters or run-time data files.
      * @param[in] enrollmentDir
      * The top-level directory in which enrollment data was placed. This
      * variable allows an implementation
      * to locate any private initialization data it elected to place in the
      * directory.
      * @param[in] edbName
      * The name of a single file containing concatenated templates, i.e. the
      * EDB described in <em>File structure for enrolled template collection</em>.
      * While the file will have read-write-delete permission, the implementation
      * should only alter the file if it preserves the necessary content, in
      * other files for example.
      * The file may be opened directly.  It is not necessary to prepend a
      * directory name.  This is a NIST-provided
      * input - implementers shall not internally hard-code or assume any values.
      * @param[in] edbManifestName
      * The name of a single file containing the EDB manifest described in
      * <em>File structure for enrolled template collection</em>.
      * The file may be opened directly.  It is not necessary to prepend a
      * directory name.  This is a NIST-provided
      * input - implementers shall not internally hard-code or assume any values.
      * @param[in] galleryType
      * The composition of the gallery as enumerated by GalleryType.
      */
    virtual FIVE::ReturnStatus
    finalizeEnrollment(
    	const std::string &configDir,
        const std::string &enrollmentDir,
        const std::string &edbName,
        const std::string &edbManifestName,
        FIVE::GalleryType galleryType) = 0;

    /**
     * @brief This function will be called once prior to one or more calls to
     * search().  The function might set static internal variables
     * and read the enrollment gallery into memory
     * so that the enrollment database is available to the subsequent
     * identification searches.
     *
     * This function will be called from a single process/thread.
     *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
     * parameters or run-time data files.
     * @param[in] enrollmentDir
     * The read-only top-level directory in which enrollment data was placed.
     */
    virtual FIVE::ReturnStatus
    initializeSearch(
        const std::string &configDir,
        const std::string &enrollmentDir) = 0;

    /**
     * @brief This function searches a probe/identification template against the
     * enrollment set, and outputs a vector containing candidateListLength
     * Candidates.
     *
     * @details Each candidate shall be populated by the implementation
     * and added to candidateList.  Note that candidateList will be an empty
     * vector when passed into this function.  The candidates shall appear in descending order
     * of similarity score - i.e., the most similar entries appear first.
     *
     * @param[in] idTemplate
     * A template from the implemented search template creation function.  If the value 
     * returned by that function was non-successful, the contents of idTemplate will not be
     * used, and this function will not be called.
     *
     * @param[in] candidateListLength
     * The number of candidates the search should return.
     * @param[out] candidateList
     * Each candidate shall be populated by the implementation.  The candidates
     * shall appear in descending order of similarity score - i.e., the most similar
     * entries appear first.
     */
    virtual FIVE::ReturnStatus
    search(
        const std::vector<uint8_t> &searchTemplate,
        const uint32_t &candidateListLength,
        std::vector<FIVE::Candidate> &candidateList) = 0;

    /**
     * @brief This function outputs the threshold that corresponds to an intended/target false positive
     * identification rate (FPIR) 
     *
     * @details This function returns what the developer thinks the threshold should be set 
     * to (not what NIST generates) for a target FPIR.  This function would be useful to end-users 
     * for system configuration.  The goal is to foster a generic threshold regardless of type of 
     * imagery, demographics, etc. that yields predictable FPIR. 
     *
     * @param[in] numPeopleInGallery
     * The number of people enrolled in a gallery/database
     *
     * @param[in] intendedFPIR
     * The target FPIR of interest
     *
     * @param[out] threshold
     * The threshold that corresponds to the intended FPIR
     */ 
    virtual FIVE::ReturnStatus
    getThreshold(
        const uint32_t &numPeopleInGallery,
        const double &intendedFPIR,
        double &threshold) = 0; 

    /**
     * @brief
     * Factory method to return a managed pointer to the Interface object.
     * @details
     * This function is implemented by the submitted library and must return
     * a managed pointer to the Interface object.
     *
     * @note
     * A possible implementation might be:
     * return (std::make_shared<Implementation>());
     */
    static std::shared_ptr<Interface>
    getImplementation();
};

/*
 * API versioning
 *
 * NIST code will extern the version number symbols.
 * Participant shall compile them into their core library.
 */
#ifdef NIST_EXTERN_API_VERSION
/** API major version number. */
extern uint16_t API_MAJOR_VERSION;
/** API minor version number. */
extern uint16_t API_MINOR_VERSION;
#else /* NIST_EXTERN_API_VERSION */
/** API major version number. */
uint16_t API_MAJOR_VERSION{1};
/** API minor version number. */
uint16_t API_MINOR_VERSION{0};
#endif /* NIST_EXTERN_API_VERSION */
}

#endif /* FRTE_FIVE_H_ */
