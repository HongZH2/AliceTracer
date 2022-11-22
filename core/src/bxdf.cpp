//
// Created by zhanghong50 on 2022/11/1.
//

#include "core/include/bxdf.h"
#include "core/include/random_variable.h"

namespace ALICE_TRACER{
    // --------------------------------------
    // Cosin weighted BRDF
    // brdf = cos<norm, light> * dw
    // --------------------------------------
    AVec3 CosinWeightedBRDF::evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) {
        // we take the cos<> term as the pdf, so that cos<> term in the render equation is cancelled.
        return mtl->albedo().ToVec3() * AVec3(M_1_PI) * AClamp(ADot(out, normal));
    }

    void CosinWeightedBRDF::sampleBxDF(ALICE_UTILS::AVec3 &sample, float &pdf, ALICE_UTILS::AVec3 point,
                                       ALICE_UTILS::AVec3 in, ALICE_UTILS::AVec3 normal, ALICE_TRACER::Material *mtl) {
        // sample Lambert BRDF = albedo/PI * cos<normal, out>
        // so that we can take the cos<> term as the pdf for sampling
        float phi = 2.f * M_PI * random_val<float>();
        float cos_theta = std::sqrt(1.f - random_val<float>());
        float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
        float x = sin_theta * std::cos(phi);
        float y = sin_theta * std::sin(phi);
        float z = cos_theta;

        // transform the outward vector from local coordinate to global coordinate
        AVec3 up;
        abs(normal.z) > 0.9999f ? up = AVec3(1.f, 0.f, 0.f) : up = AVec3(0.f, 0.f, 1.f);
        AVec3 tangent = ANormalize(ACross(up, normal));
        AVec3 bitangent = ACross(normal, tangent);

        // return the sample direction and the pdf
        sample = tangent * x + bitangent * y + normal * z;
        pdf = AClamp(ADot(sample, normal)) * (float)M_1_PI;
    }

    float CosinWeightedBRDF::samplePDF(ALICE_UTILS::AVec3 dir, ALICE_UTILS::AVec3 in, ALICE_UTILS::AVec3 normal,
                                       ALICE_TRACER::Material *mtl) {
        // given a direction, return the pdf
        // Lambert BRDF PDF: cos<dir, normal> / PI
        return AClamp(ADot(dir, normal)) * (float)M_1_PI;
    }

    // --------------------------------------
    // Perfect Mirrored BRDF
    // --------------------------------------
    AVec3 PerfectMirroredBRDF::evaluateBxDF(ALICE_UTILS::AVec3 point, ALICE_UTILS::AVec3 normal, ALICE_UTILS::AVec3 in,
                                            ALICE_UTILS::AVec3 out, ALICE_TRACER::Material *mtl) {
        // for mirrored BRDF
        AVec3 refl = ANormalize(2.f * AClamp(ADot(-in, normal)) * normal + in);
        if( abs(ADot(out, refl) - 1.f) < MIN_THRESHOLD){  // it should be same as the reflection
            return mtl->albedo().ToVec3() * AVec3(M_1_PI) * AClamp(ADot(out, normal));
        }
        return AVec3{0.f};
    }

    float PerfectMirroredBRDF::samplePDF(ALICE_UTILS::AVec3 dir, ALICE_UTILS::AVec3 in, ALICE_UTILS::AVec3 normal,
                                         ALICE_TRACER::Material *mtl) {
        return 0.f;
    }

    void PerfectMirroredBRDF::sampleBxDF(ALICE_UTILS::AVec3 &sample, float &pdf, ALICE_UTILS::AVec3 point,
                                         ALICE_UTILS::AVec3 in, ALICE_UTILS::AVec3 normal,
                                         ALICE_TRACER::Material *mtl) {
        sample = ANormalize(2.f * AClamp(ADot(-in, normal)) * normal + in);
        pdf = 1.f;
    }


}