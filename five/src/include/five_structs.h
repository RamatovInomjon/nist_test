/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FIVE_STRUCTS_H_
#define FIVE_STRUCTS_H_

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace FIVE {
/**
 * @brief
 * Struct representing a single image
 */
typedef struct Image {
    /** Labels describing the type of image */

    // ok so we have still/video below so remove the "Still"/"Video" from enum below

    enum class ImageDescription {
        /** Image/frame with unknown or unassigned collection conditions.
         *  Media labeled as "Unknown" could include any category of imagery, 
         *  and developers are expected to handle this appropriately.
         */
        Unknown = 0,
        /** Face image, Frontal closely ISO/IEC 19794-5:2005 compliant */
        StillISO = 1,
        /** Face image from law enforcement booking processes, nominally frontal */
        StillMugshot = 2,
        /** Face image that might appear in a news source or magazine. The images are typically
         * typically well exposed and focused but exhibit pose and illumination variations. */
        StillPhotojournalism = 3,
        /** Unconstrained face, taken by amateur photographer, widely varying pose, illumination, resolution */
        StillWild = 4,
        /** Frame from video collected at long range (and potentially under turbulent conditions) */
        VideoLongRange = 5,
        /** Frame from video from television footage */ 
        VideoPhotojournalism = 6,
        /** Frame from video collected passively in spaces such as sports arenas, airports, etc.) */
        VideoPassiveObservation = 7,
        /** Frame from video collected at a chokepoint */
        VideoChokepoint = 8,
        /** Frame from video collected from elevated platforms for which look-down angle is large */
        VideoElevatedPlatform = 9
    };  
       
    enum class Illuminant {
        /** Not specified */
        Unspecified = 0,
        /** Conventional visible light */
        Visible = 1,
    };

    /** Number of pixels horizontally */
    uint16_t width;
    /** Number of pixels vertically */
    uint16_t height;
    /** Number of bits per pixel. Legal values are 8 and 24. */
    uint8_t depth;
    /** Managed pointer to raster scanned data.
     * Either RGB color or intensity.
     * If image_depth == 24 this points to  3WH bytes  RGBRGBRGB...
     * If image_depth ==  8 this points to  WH bytes  IIIIIII */
    std::shared_ptr<uint8_t> data;
    /** Single description of the image.  */     
    ImageDescription description;
    /** Source of light used to acquire the image */
    Illuminant illuminant;

    Image() :
        width{0},
        height{0},
        depth{24},
        description{ImageDescription::Unknown},
        illuminant{Illuminant::Visible}
        {}

    Image(
        uint16_t width,
        uint16_t height,
        uint8_t depth,
        std::shared_ptr<uint8_t> &data,
        ImageDescription description,
        Illuminant illuminant
        ) :
        width{width},
        height{height},
        depth{depth},
        data{data},
        description{description},
        illuminant{illuminant}
        {}

    /** @brief This function returns the size of the image data. */
    size_t
    size() const { return (width * height * (depth / 8)); }
} Image;

/**
 * @brief
 * Struct representing a piece of media
 */
typedef struct Media {
    /** Labels describing the type of media */
    enum class Label {
        /** Still photos of an individual */
        Image = 0,
        /** Sequential/chronological video frames of an individual */
        Video = 1
    };

    /** Type of media */
    Label type;
    /** Vector of still image(s) or video frames in chronological order */
    std::vector<FIVE::Image> data;
    /** For video data, the frame rate in frames per second */
    uint8_t fps;

    Media() :
        type{Media::Label::Image},
        fps{0}
        {}

    Media(
        const Media::Label type,
        const std::vector<FIVE::Image> &data,
        const uint8_t fps
        ) :
        type{type},
        data{data},
        fps{fps}
        {}
} Media;

/** Labels describing the composition of the 1:N gallery
 *  (provided as input into gallery finalization function)
 */
enum class GalleryType {
    /** Consolidated, subject-based */
    Consolidated = 0,
    /** Unconsolidated, event-based */
    Unconsolidated = 1
};

/**
 * @brief
 * Data structure for result of an identification search
 */
typedef struct Candidate {
    /** @brief If the candidate is valid, this should be set to true. If
     * the candidate computation failed, this should be set to false.
     * If value is set to false, score and templateId
     * will be ignored entirely. */
    bool isAssigned;

    /** @brief The template ID from the enrollment database manifest */
    std::string templateId;

    /** @brief Measure of similarity or dissimilarity between the identification template
     * and the enrolled candidate.
     * For face recognition, a similarity score - higher is more similar;
     * For iris recognition, a non-negative measure of dissimilarity (maybe a distance) - lower is more similar;
     * For multimodal face and iris, a similarity score - higher is more similar;
     */
    double score;

    Candidate() :
        isAssigned{false},
        templateId{""},
        score{-1.0}
        {}

    Candidate(
        bool isAssigned,
        std::string templateId,
        double score) :
        isAssigned{isAssigned},
        templateId{templateId},
        score{score}
        {}
} Candidate;

/**
 * @brief
 * Return codes for functions specified in this API
 */
enum class ReturnCode {
    /** Success */
    Success = 0,
    /** Catch-all error */
    UnknownError = 1,
    /** Error reading configuration files */
    ConfigError = 2,
    /** Elective refusal to process the input */
    RefuseInput = 3,
    /** Involuntary failure to process the image */
    ExtractError = 4,
    /** Cannot parse the input data */
    ParseError = 5,
    /** Elective refusal to produce a template */
    TemplateCreationError = 6,
    /** Either or both of the input templates were result of failed feature extraction */
    VerifTemplateError = 7,
    /** Unable to detect a face in the image */
    FaceDetectionError = 8,
    /** The implementation cannot support the number of input images */
    NumDataError = 9,
    /** Template file is an incorrect format or defective */
    TemplateFormatError = 10,
    /** An operation on the enrollment directory failed (e.g. permission, space) */
    EnrollDirError = 11,
    /** Cannot locate the input data - the input files or names seem incorrect */
    InputLocationError = 12,
    /** Memory allocation failed (e.g. out of memory) */
    MemoryError = 13,
    /** Function is not implemented */
    NotImplemented = 14,
    /** Vendor-defined failure */
    VendorError = 15
};

/** Output stream operator for a ReturnCode object. */
inline std::ostream&
operator<<(
    std::ostream &s,
    const ReturnCode &rc)
{
    switch (rc) {
    case ReturnCode::Success:
        return (s << "Success");
    case ReturnCode::UnknownError:
        return (s << "Unknown Error");
    case ReturnCode::ConfigError:
        return (s << "Error reading configuration files");
    case ReturnCode::RefuseInput:
        return (s << "Elective refusal to process the input");
    case ReturnCode::ExtractError:
        return (s << "Involuntary failure to process the image");
    case ReturnCode::ParseError:
        return (s << "Cannot parse the input data");
    case ReturnCode::TemplateCreationError:
        return (s << "Elective refusal to produce a template");
    case ReturnCode::VerifTemplateError:
        return (s << "Either or both of the input templates were result of "
                "failed feature extraction");
    case ReturnCode::FaceDetectionError:
        return (s << "Unable to detect a face in the image");
    case ReturnCode::NumDataError:
        return (s << "Number of input images not supported");
    case ReturnCode::TemplateFormatError:
        return (s << "Template file is an incorrect format or defective");
    case ReturnCode::EnrollDirError:
        return (s << "An operation on the enrollment directory failed");
    case ReturnCode::InputLocationError:
        return (s << "Cannot locate the input data - the input files or names "
                "seem incorrect");
    case ReturnCode::MemoryError:
        return (s << "Memory allocation failed (e.g. out of memory)");
    case ReturnCode::NotImplemented:
        return (s << "Function is not implemented");
    case ReturnCode::VendorError:
        return (s << "Vendor-defined error");
    default:
        return (s << "Undefined error");
    }
}

/**
 * @brief
 * A structure to contain information about a failure by the software
 * under test.
 *
 * @details
 * An object of this class allows the software to return some information
 * from a function call. The string within this object can be optionally
 * set to provide more information for debugging etc. The status code
 * will be set by the function to Success on success, or one of the
 * other codes on failure.
 */
typedef struct ReturnStatus {
    /** @brief Return status code */
    ReturnCode code;
    /** @brief Optional information string */
    std::string info;

    ReturnStatus() :
        code{ReturnCode::UnknownError},
        info{""}
        {}
    /**
     * @brief
     * Create a ReturnStatus object.
     *
     * @param[in] code
     * The return status code; required.
     * @param[in] info
     * The optional information string.
     */
    ReturnStatus(
        const ReturnCode code,
        const std::string &info = ""
        ) :
        code{code},
        info{info}
        {}
} ReturnStatus;

typedef struct BoundingBox
{
    /** @brief leftmost point on head, typically subject's right ear
     *  value must be on [0, imageWidth-1] */
    int16_t xleft;
    /** @brief high point of head, typically top of hair
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

/*
* Versioning
*
* NIST code will extern the version number symbols. Participant
* shall compile them into their core library.
*/
#ifdef NIST_EXTERN_FIVE_STRUCTS_VERSION
/** major version number. */
extern uint16_t FIVE_STRUCTS_MAJOR_VERSION;
/** minor version number. */
extern uint16_t FIVE_STRUCTS_MINOR_VERSION;
#else /* NIST_EXTERN_FIVE_STRUCTS_VERSION */
/** major version number. */
uint16_t FIVE_STRUCTS_MAJOR_VERSION{1};
/** minor version number. */
uint16_t FIVE_STRUCTS_MINOR_VERSION{0};
#endif /* NIST_EXTERN_FIVE_STRUCTS_VERSION */
}

#endif /* FIVE_STRUCTS_H_ */
