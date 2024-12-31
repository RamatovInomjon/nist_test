/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRVT_MORPH_H_
#define FRVT_MORPH_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <frvt_structs.h>

namespace FRVT_MORPH {
/** Labels describing image media type */
enum class ImageLabel {
    /** Image type is unknown or unassigned */
    Unknown = 0,
    /** Non-scanned image */
    NonScanned = 1,
    /** Printed-and-scanned image */
    Scanned = 2
};

/**
 * @brief
 * Struct representing metadata, including
 * subject sex, age of subject in probe image,
 * and age/time difference between probe and
 * reference images 
 */
typedef struct SubjectMetadata {
    /** Labels for subject sex */
    enum class Sex {
        /** Unknown or unassigned */
        Unknown = 0,
        Female,
        Male
    };
     
    /** Sex of subject */
    Sex sex;
    /** Age of subject (in months) in probe image
     *  -1 indicates an unassigned value */
    int16_t ageInMonths;
    /** Age/time difference (in months) between probe
     * and reference image; -1 indicates an unassigned value */
    int16_t ageDeltaInMonths;    

    SubjectMetadata() :
        sex{Sex::Unknown},
        ageInMonths{-1},
        ageDeltaInMonths{-1}
        {}

    SubjectMetadata(
        Sex sex,
        int16_t ageInMonths,
        int16_t ageDeltaInMonths
        ) :
        sex{sex},
        ageInMonths{ageInMonths},
        ageDeltaInMonths{ageDeltaInMonths}
        {}
} SubjectMetadata;

/**
 * @brief
 * The interface to FRVT MORPH implementation
 *
 * @details
 * The submission software under test will implement this interface by
 * sub-classing this class and implementing each method therein.
 */
class Interface {
public:
    virtual ~Interface() {}

    /**
     * @brief Before images are sent to any morph detection or match function,
     * the test harness will call this initialization function.
     * @details This function will be called N=1 times by the NIST application,
     * prior to parallelizing M >= 1 calls to morph detection or matching
     * functions via fork().
     *
     * This function will be called from a single process/thread.
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.
     *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
     * parameters or run-time data files.
     * @param[in] configValue
     * An optional string value encoding developer-specific configuration parameters
     */
    virtual FRVT::ReturnStatus
    initialize(
        const std::string &configDir,
        const std::string &configValue) = 0;

    /**
     * @brief This function takes an input image and outputs
     * 1. a binary decision on whether the image is a morph
     * 2. a "morphiness" score on [0, 1] indicating how confident the algorithm
     * thinks the image is a morph, with 0 meaning confidence that the image
     * is not a morph and 1 representing absolute confidence that it is a morph
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.  If this function is not implemented for
     * a certain type of image, for example, the function supports non-scanned
     * photos but not scanned photos, then the function should return
     * ReturnCode::NotImplemented when the function is called with the particular
     * unsupported image type.
     *
     * @param[in] suspectedMorph
     * Input image
     * @param[in] label
     * Label indicating the type of imagery for the suspected morph.  Possible
     * types are non-scanned photo, printed-and-scanned photo, or unknown.
     * @param[out] isMorph
     * True if image contains a morph; False otherwise
     * @param[out] score
     * A score on [0, 1] representing how confident the algorithm is that the
     * image contains a morph.  0 means certainty that image does not contain
     * a morph and 1 represents certainty that image contains a morph
     */
    virtual FRVT::ReturnStatus
    detectMorph(
        const FRVT::Image &suspectedMorph,
        const FRVT_MORPH::ImageLabel &label,
        bool &isMorph,
        double &score) = 0;

    /**
     * @brief This function takes two input images - a known unaltered/not morphed
     * image of the subject and an image of the same subject that's in question
     * (may or may not be a morph).  This function outputs
     * 1. a binary decision on whether <b>suspectedMorph</b> is a morph
     * (given <b>probeFace</b> as a prior)
     * 2. a "morphiness" score on [0, 1] indicating how confident the algorithm
     * thinks the image is a morph, with 0 meaning confidence that the image
     * is not a morph and 1 representing absolute confidence that it is a morph
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.  If this function is not implemented for
     * a certain type of image, for example, the function supports non-scanned
     * photos but not scanned photos, then the function should return
     * ReturnCode::NotImplemented when the function is called with the particular
     * unsupported image type.
     *
     * @param[in] suspectedMorph
     * An image in question of being a morph (or not)
     * @param[in] label
     * Label indicating the type of imagery for the suspected morph.  Possible
     * types are non-scanned photo, printed-and-scanned photo, or unknown.
     * @param[in] probeFace
     * An image of the subject known not to be a morph (i.e., live capture
     * image)
     * @param[out] isMorph
     * True if suspectedMorph image contains a morph; False otherwise
     * @param[out] score
     * A score on [0, 1] representing how confident the algorithm is that the
     * image contains a morph.  0 means certainty that image does not contain
     * a morph and 1 represents certainty that image contains a morph
     */
    virtual FRVT::ReturnStatus
    detectMorphDifferentially(
        const FRVT::Image &suspectedMorph,
        const FRVT_MORPH::ImageLabel &label,
        const FRVT::Image &probeFace,
        bool &isMorph,
        double &score) = 0;

    /**
     * @brief This function takes two images and information 
     * about the subject as input.  The inputs are a known unaltered/not morphed
     * image of the subject, an image of the same subject that's in question
     * (may or may not be a morph), and subject metadata (sex, age,
     * age/time difference between probe and reference image).  This function outputs
     * 1. a binary decision on whether <b>suspectedMorph</b> is a morph
     * (given <b>probeFace</b> as a prior)
     * 2. a "morphiness" score on [0, 1] indicating how confident the algorithm
     * thinks the image is a morph, with 0 meaning confidence that the image
     * is not a morph and 1 representing absolute confidence that it is a morph
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.  
     *
     * @param[in] suspectedMorph
     * An image in question of being a morph (or not)
     * @param[in] label
     * Label indicating the type of imagery for the suspected morph.  Possible
     * types are non-scanned photo, printed-and-scanned photo, or unknown.
     * @param[in] probeFace
     * An image of the subject known not to be a morph (i.e., live capture
     * image)
     * @param[in] subjectMetadata
     * Information about the subject, including sex, subject's age in the probe image,
     * and age/time difference between the suspected morph and the probe image  
     * @param[out] isMorph
     * True if suspectedMorph image contains a morph; False otherwise
     * @param[out] score
     * A score on [0, 1] representing how confident the algorithm is that the
     * image contains a morph.  0 means certainty that image does not contain
     * a morph and 1 represents certainty that image contains a morph
     */
    virtual FRVT::ReturnStatus
    detectMorphDifferentially(
        const FRVT::Image &suspectedMorph,
        const FRVT_MORPH::ImageLabel &label,
        const FRVT::Image &probeFace,
        const FRVT_MORPH::SubjectMetadata &subjectMetadata,
        bool &isMorph,
        double &score) = 0;

    /**
     * @brief This function compares two images and outputs a
     * similarity score. In the event the algorithm cannot perform the comparison
     * operation, the similarity score shall be set to -1 and the function
     * return code value shall be set appropriately.
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.
     *
     * param[in] enrollImage
     * The enrollment image
     * param[in] verifImage
     * The verification image
     * param[out] similarity
     * A similarity score resulting from comparison of the two images,
     * on the range [0,DBL_MAX].
     *
     */
    virtual FRVT::ReturnStatus
    compareImages(
        const FRVT::Image &enrollImage,
        const FRVT::Image &verifImage,
        double &similarity) = 0;


    /**
     * @brief This function takes an input image and outputs two images.  
     * If the input image is a morph, the algorithm should deduce/restore the two 
     * individual face images/identities that contributed to the morph.  If the 
     * input is a bona fide image, the algorithm should produce two images that are 
     * essentially the same as the input photo. All morphs will be generated with two 
     * contributing subjects.
     *
     * Optionally, the algorithm can also return a binary decision on whether the 
     * image is a morph and a "morphiness" score on [0, 1] indicating how confident 
     * the algorithm thinks the image is a morph, with 0 meaning confidence that the 
     * image is not a morph and 1 representing absolute confidence that it is a morph.  
     * A score of -1.0 indicates that the algorithm did not implement morph detection 
     * and both "isMorph" and "score" will be ignored.
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.  
     *
     * @param[in] suspectedMorph
     * Input image
     * @param[out] outputSubject1
     * If the input image is a morph, the algorithm should return the first 
     * individual face image/identity that contributed to the morph.  If the input is a 
     * bona fide image, the algorithm should produce an image that is essentially the 
     * same as the input photo.
     * @param[out] outputSubject2
     * If the input image is a morph, the algorithm should return the second
     * individual face image/identity that contributed to the morph.  If the input is a 
     * bona fide image, the algorithm should produce an image that is essentially the 
     * same as the input photo.
     * @param[out] isMorph
     * (optional) True if image contains a morph; False otherwise
     * @param[out] score
     * (optional) A score on [0, 1] representing how confident the algorithm is that the
     * input image contains a morph.  0 means certainty that image does not contain
     * a morph and 1 represents certainty that image contains a morph
     */
    virtual FRVT::ReturnStatus
    demorph(
        const FRVT::Image &suspectedMorph,
        FRVT::Image &outputSubject1,
        FRVT::Image &outputSubject2,
        bool &isMorph,
        double &score) = 0;

    /**
     * @brief This function takes two input images - a known unaltered/not morphed image
     * of the subject (probeFace) and an image of the same subject that's in question (suspectedMorph).  
     * If the input image is a morph, the algorithm should deduce/restore the other/unknown 
     * individual face image/identity that contributed to the morph.  If the input is a bona fide 
     * image, the algorithm should produce an image that is essentially the same as the input photo.
     * 
     * Optionally, the algorithm can also return a binary decision on whether the image is a morph 
     * and a "morphiness" score on [0, 1] indicating how confident the algorithm thinks the image 
     * is a morph, with 0 meaning confidence that the image is not a morph and 1 representing absolute 
     * confidence that it is a morph.  A score of -1.0 indicates that the algorithm did not implement 
     * morph detection and both "isMorph" and "score" will be ignored.
     *
     * If this function is not implemented, the algorithm shall return
     * ReturnCode::NotImplemented.
     *
     * @param[in] suspectedMorph
     * Input image
     * @param[in] probeFace
     * An image of the subject known not to be a morph (i.e., live capture
     * image)
     * @param[out] outputSubject
     * If the input image is a morph, the algorithm should deduce/restore the other/unknown 
     * individual face image/identity that contributed to the morph.  If the input is a bona 
     * fide image, the algorithm should produce an image that is essentially the same as the input photo.
     * @param[out] isMorph
     * (optional) True if image contains a morph; False otherwise
     * @param[out] score
     * (optional) A score on [0, 1] representing how confident the algorithm is that the
     * input image contains a morph.  0 means certainty that image does not contain
     * a morph and 1 represents certainty that image contains a morph
     */
    virtual FRVT::ReturnStatus
    demorphDifferentially(
        const FRVT::Image &suspectedMorph,
        const FRVT::Image &probeFace,
        FRVT::Image &outputSubject,
        bool &isMorph,
        double &score) = 0;

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
uint16_t API_MAJOR_VERSION{5};
/** API minor version number. */
uint16_t API_MINOR_VERSION{0};
#endif /* NIST_EXTERN_API_VERSION */
}

#endif /* FRVT_MORPH_H_ */
