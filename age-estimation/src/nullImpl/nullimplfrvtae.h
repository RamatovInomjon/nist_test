/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef NULLIMPLFRVT11_H_
#define NULLIMPLFRVT11_H_

#include "frvt_ae.h"

/*
 * Declare the implementation class of the FRVT AE(Age Estimation) Interface
 */
namespace FRVT_AE {
    class NullImplFRVTAE : public FRVT_AE::Interface {
public:

    NullImplFRVTAE();
    ~NullImplFRVTAE() override;

    FRVT::ReturnStatus
    initialize(const std::string &configDir) override;

    FRVT::ReturnStatus
    estimateAge(
            const FRVT::Media &face,
            double &age) override;

    FRVT::ReturnStatus
    estimateAge(
            const FRVT::Media &faceOnei,
	    const double &ageOne,
            const FRVT::Media &faceTwo,
            double &age) override;

    FRVT::ReturnStatus
    verifyAge(
        const FRVT::Media &face,
        const double &ageThreshold,
        bool &isAboveThreshold) override;

    static std::shared_ptr<FRVT_AE::Interface>
    getImplementation();

private:
    std::string configDir;
    static const int featureVectorSize{4};
    // Some other members
};
}

#endif /* NULLIMPLFRVTAE_H_ */
