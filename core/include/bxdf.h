//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_BXDF_H
#define ALICE_TRACER_BXDF_H

#include "material.h"

namespace ALICE_TRACER{
    /*
     * Define the BxDF base class
     */
    class BxDFBase{
    public:
        BxDFBase() = default;
        virtual ~BxDFBase() = default;

        /*
         * evaluate the BxDF
         */
        virtual AVec3 evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) = 0;
        /*
         * uniform sample/importance sample by BxDF
         */
        virtual AVec3 sampleBxDF(AVec3 point, AVec3 normal, Material * mtl) = 0;
    };

    /*
     * Define the BRDF class
     */
    class LambertBRDF: public BxDFBase{
    public:
        LambertBRDF() = default;
        ~LambertBRDF() override = default;

        AVec3 evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) override;
        AVec3 sampleBxDF(AVec3 point, AVec3 normal, Material * mtl) override;
    };


}

#endif //ALICE_TRACER_BXDF_H
