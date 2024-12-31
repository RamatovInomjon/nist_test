/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRVT_QUALITY_H_
#define FRVT_QUALITY_H_

#include <cstdint>
#include <string>
#include <vector>

#include <frvt_structs.h>

namespace FRVT_QUALITY {

/** Quality measure labels
 */
enum class QualityMeasure {
    Begin = 0,
    TotalFacesPresent = Begin,
    SubjectPoseRoll,
    SubjectPosePitch,
    SubjectPoseYaw,
    EyeGlassesPresent,
    SunGlassesPresent,
    Underexposure,
    Overexposure,
    BackgroundUniformity,
    MouthOpen,
    MouthOpen2,
    EyesOpen,
    EyesOpen2,
    FaceOcclusion,
    FaceOcclusion2,
    Resolution,
    InterEyeDistance,
    MotionBlur,
    CompressionArtifacts,
    PixelsFromEyeToLeftEdge,
    PixelsFromEyeToRightEdge,
    PixelsFromEyesToBottom,
    PixelsFromEyesToTop,
    UnifiedQualityScore,
    End
};


/** To support iterating over QualityMeasure enum values */
inline QualityMeasure&
operator++(QualityMeasure& qe) {
   if (qe == QualityMeasure::End)
        throw std::out_of_range("QualityMeasure& operator++(QualityMeasure&)");
    qe = QualityMeasure(static_cast<std::underlying_type<QualityMeasure>::type>(qe) + 1);
    return qe;
}

/** Output stream operator for QualityMeasure enum. */
inline std::ostream&
operator<<(
    std::ostream &s,
    const QualityMeasure &qe)
{
    switch (qe) {
    case QualityMeasure::TotalFacesPresent:
        return (s << "TotalFacesPresent");
    case QualityMeasure::SubjectPoseRoll:
        return (s << "SubjectPoseRoll");
    case QualityMeasure::SubjectPosePitch:
        return (s << "SubjectPosePitch");
    case QualityMeasure::SubjectPoseYaw:
        return (s << "SubjectPoseYaw");
    case QualityMeasure::EyeGlassesPresent:
        return (s << "EyeGlassesPresent");
    case QualityMeasure::SunGlassesPresent:
        return (s << "SunGlassesPresent");
    case QualityMeasure::Underexposure:
        return (s << "Underexposure");
    case QualityMeasure::Overexposure:
        return (s << "Overexposure");
    case QualityMeasure::BackgroundUniformity:
        return (s << "BackgroundUniformity");
    case QualityMeasure::MouthOpen:
        return (s << "MouthOpen");
    case QualityMeasure::MouthOpen2:
        return (s << "MouthOpen2");
    case QualityMeasure::EyesOpen:
        return (s << "EyesOpen");
    case QualityMeasure::EyesOpen2:
        return (s << "EyesOpen2");
    case QualityMeasure::FaceOcclusion:
        return (s << "FaceOcclusion");
    case QualityMeasure::FaceOcclusion2:
        return (s << "FaceOcclusion2");
    case QualityMeasure::Resolution:
        return (s << "Resolution");
    case QualityMeasure::InterEyeDistance:
        return (s << "InterEyeDistance");
    case QualityMeasure::MotionBlur:
        return (s << "MotionBlur");
    case QualityMeasure::CompressionArtifacts:
        return (s << "CompressionArtifacts");
    case QualityMeasure::PixelsFromEyeToLeftEdge:
        return (s << "PixelsFromEyeToLeftEdge");
    case QualityMeasure::PixelsFromEyeToRightEdge:
        return (s << "PixelsFromEyeToRightEdge");
    case QualityMeasure::PixelsFromEyesToBottom:
        return (s << "PixelsFromEyesToBottom");
    case QualityMeasure::PixelsFromEyesToTop:
        return (s << "PixelsFromEyesToTop");
    case QualityMeasure::UnifiedQualityScore:
        return (s << "UnifiedQualityScore");
    default:
        return (s << "undefined QualityMeasure");
    }
}

/**
 * @brief
 * Data structure that stores key-value pairs, with each
 * entry representing a quality element and its value
 */
using QualityAssessments = std::map<QualityMeasure, double>;

typedef struct BoundingBox
{
    /** @brief leftmost point on head, typically subject's right ear
     *  value must be on [0, imageWidth-1] */
    int16_t xleft;
    /** @brief high point of head, typically top of hair;
     *  value must be on [0, imageHeight-1] */
    int16_t ytop;
    /** @brief bounding box width */
    int16_t width;
    /** @brief bounding box height */
    int16_t height;

    BoundingBox() :
        xleft{-1},
        ytop{-1},
        width{-1},
        height{-1}
        {}

    BoundingBox(
        int16_t xleft,
        int16_t ytop,
        int16_t width,
        int16_t height) :
        xleft{xleft},
        ytop{ytop},
        width{width},
        height{height}
        {}
} BoundingBox;

typedef struct ImageQualityAssessment
{
    BoundingBox boundingBox;
    QualityAssessments qAssessments;

    ImageQualityAssessment() :
        boundingBox{},
        qAssessments{}
        {}

    ImageQualityAssessment(
        const BoundingBox &boundingBox,
        const QualityAssessments &qAssessments) :
        boundingBox{boundingBox},
        qAssessments{qAssessments}
        {}
} ImageQualityAssessment;

/**
 * @brief
 * The interface to FRVT QUALITY implementation
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
     * @brief  This function takes an image and outputs
     * face location and quality information. The quality assessment   
     * should be performed on the largest detected face.
     *
     * @param[in] image
     * Single face image
     *
     * @param[out] assessments
     * An ImageQualityAssessments structure. 
     * The implementation should populate
     * 1) the bounding box and
     * 2) those items in the QualityAssessments object that the
     * developer chooses to implement
     */ 
    virtual FRVT::ReturnStatus
    vectorQuality(
        const FRVT::Image &image,
        FRVT_QUALITY::ImageQualityAssessment &assessments) = 0;

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
uint16_t API_MAJOR_VERSION{4};
/** API minor version number. */
uint16_t API_MINOR_VERSION{1};
#endif /* NIST_EXTERN_API_VERSION */
}

#endif /* FRVT_QUALITY_H_ */
