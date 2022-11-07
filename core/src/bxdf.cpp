//
// Created by zhanghong50 on 2022/11/1.
//

#include "core/include/bxdf.h"
#include "core/include/random_variable.h"

namespace ALICE_TRACER{

    AVec3 LambertBRDF::evaluateBxDF(AVec3 point, AVec3 normal, AVec3 in, AVec3 out, Material * mtl) {
        // we take the cos<> term as the pdf, so that cos<> term in the render equation is cancelled.
        if(ADot(out, normal) < MIN_THRESHOLD)
            return AVec3(0.f);
        return mtl->albedo().ToVec3();
    }

    AVec3 LambertBRDF::sampleBxDF(AVec3 point, AVec3 normal, Material * mtl) {
        // sample Lambert BRDF = albedo/PI * cos<normal, out>
        // so that we can take the cos<> term as the pdf for sampling
        float phi = 2.f * M_PI * random_val<float>();
        float cos_theta = sqrt(1.f - random_val<float>());
        float sin_theta = sqrt(1.f - cos_theta * cos_theta);
        float x = sin_theta * cos(phi);
        float y = sin_theta * sin(phi);
        float z = cos_theta;

        // transform the outward vector from local coordinate to global coordinate
        AVec3 up;
        abs(normal.z) > 0.9999f ? up = AVec3(1.f, 0.f, 0.f) : up = AVec3(0.f, 0.f, 1.f);
        AVec3 tangent = normalize(cross(up, normal));
        AVec3 bitangent = cross(normal, tangent);

        return tangent * x + bitangent * y + normal * z;
    }

}