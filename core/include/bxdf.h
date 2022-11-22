//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_BXDF_H
#define ALICE_TRACER_BXDF_H

#include "material.h"

namespace ALICE_TRACER{
    // --------------------------------------
    // BxDF Definition
    // --------------------------------------
    class BxDFBase{
    public:
        BxDFBase() = default;
        virtual ~BxDFBase() = default;
         // evaluate the BxDF
        virtual AVec3 evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) = 0;
        // sample the BxDF
        virtual void sampleBxDF(AVec3 & sample, float & pdf, AVec3 point, AVec3 in, AVec3 normal, Material * mtl) = 0;
        // compute the pdf of the sample direction
        virtual float samplePDF(AVec3 dir, AVec3 in, AVec3 normal, Material * mtl) = 0;
    };

    // --------------------------------------
    // Cosin weighted BRDF
    // brdf = cos<norm, light> * dw
    // --------------------------------------
    class CosinWeightedBRDF: public BxDFBase{
    public:
        CosinWeightedBRDF() = default;
        ~CosinWeightedBRDF() override = default;
        AVec3 evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) override;
        void sampleBxDF(AVec3 & sample, float & pdf, AVec3 point, AVec3 in, AVec3 normal, Material * mtl) override;
        float samplePDF(AVec3 dir, AVec3 in, AVec3 normal, Material * mtl) override;
    };

    // --------------------------------------
    // Perfect Mirrored BRDF
    // --------------------------------------
    class PerfectMirroredBRDF: public BxDFBase{
    public:
        PerfectMirroredBRDF() = default;
        ~PerfectMirroredBRDF() override = default;
        AVec3 evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) override;
        void sampleBxDF(AVec3 & sample, float & pdf, AVec3 point, AVec3 in, AVec3 normal, Material * mtl) override;
        float samplePDF(AVec3 dir, AVec3 in, AVec3 normal, Material * mtl) override;
    };




}

#endif //ALICE_TRACER_BXDF_H
