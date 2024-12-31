/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRVT_AE_H_
#define FRVT_AE_H_

#include <cstdint>
#include <string>
#include <vector>

#include <frvt_structs.h>

namespace FRVT_AE {

/**
 * @brief
 * The interface to FRVT AE (Age Estimation) implementation
 *
 * @details
 * The submission software under test will implement this interface by
 * sub-classing this class and implementing each method therein.
 */
class Interface {
public:
    virtual ~Interface() {}

    /**
     * @brief This function initializes the implementation under test.  It will
     * be called by the NIST application before any call to createTemplate() or
     * matchTemplates().  The implementation under test should set all parameters.
     * This function will be called N=1 times by the NIST application, prior to
     * parallelizing M >= 1 calls to createTemplate() via fork().
     *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
     * parameters or run-time data files.  The name of this directory is
     * assigned by NIST, not hardwired by the provider.  The names of the
     * files in this directory are hardwired in the implementation and are
     * unrestricted.
     */
    virtual FRVT::ReturnStatus
    initialize(const std::string &configDir) = 0;

    /**
     * @brief This function takes an input piece of media an image or a sequential video frames  
     * and outputs an estimate age.
     *
     * @param[in] face
     * Input media of an image or a sequential video frames of one person. 
     * @param[out] age
     * An age estimation of the face in the image or sequential video frames  The legal values are [0,100] 
     */
    virtual FRVT::ReturnStatus
    estimateAge(
        const FRVT::Media &face,
        double &age) = 0;

    /**
     * @brief This function estimates the age of the person in faceTwo.  This function allows implementation 
     * to either estimate age using only the input media with faceTwo, or to exploit additional information 
     * provided with faceOne image. Developer should not assume that faceTwo is collected at a later date than faceOne. 
     *
     * @param[in] faceOne
     * Input media of an image or a sequential video frames of one person.
     * @param[in] age
     * The age of the person in faceOne.
     * @param[in] faceTwo
     * Single face image
     * @param[out] age of the person in faceTwo
     * An age estimation of the face in the image or sequential video frames with given ground truth 
     * from separate media.  The legal absolute values are [0,100]
     */
    virtual FRVT::ReturnStatus
    estimateAge(
	const FRVT::Media &faceOne,
	const double &ageOne,
	const FRVT::Media &faceTwo,
        double &age) = 0;

     /**
     * @brief This function returns a binary decision on whether the face in the image is above an age threshold.  
     * This function prototype allows an implementation to invoke specialized processing for certain age groups.  
     * We anticipate calling this function with ageThreshold values in {12, 18, 21, and 70}.  
     * We may use other values also. 
     *
     * @param[in] face
     * Input media of an image or a sequential video frames of one person.
     * @param[in] ageThreshold
     * Input age of interest. 
     * @param[out] isAboveThreshold
     * True if the estimated age of the input media is above the age threshold; False otherwise. 
     */
    virtual FRVT::ReturnStatus
    verifyAge(
        const FRVT::Media &face,
        const double &ageThreshold,
        bool &isAboveThreshold) = 0;

    /**
     * @brief
     * Factory method to return a managed pointer to the Interface object.
     * @details
     * This function is implemented by the submitted library and must return
     * a managed pointer to the Interface object.
     *
     * This function MUST be implemented.
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

#endif /* FRVT_QUALITY_H_ */
