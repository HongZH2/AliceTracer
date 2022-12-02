//
// Created by zhanghong50 on 2022/11/1.
//

#include "core/include/bxdf.h"
#include "core/include/ray.h"
#include "core/include/random_variable.h"
#include "core/include/sampler.h"

namespace ALICE_TRACER{

    // --------------------------------------
    // Fresnel Term
    // reference: https://www.pbr-book.org/3ed-2018/Reflection_Models/Specular_Reflection_and_Transmission#FrDielectric
    // --------------------------------------
    float FresnelDielectric::FrDielectric(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                            const ALICE_TRACER::Ray &out) {
        auto mtl = dynamic_cast<FresnelMaterial *>(hit_res.mtl_);
        float eta_i = mtl->eta0();
        float eta_t = mtl->eta1();
        if(hit_res.is_inside_){
            std::swap(eta_i, eta_t);
        }

        float cos_theta_i = -ADot(in.dir_, hit_res.normal_);
        float sin2_theta_i = 1.f - cos_theta_i * cos_theta_i;
        float sin2_theta_t = eta_i/eta_t * eta_i/eta_t * sin2_theta_i;
        float cos_theta_t = std::sqrt(std::max(0.f, 1.f - sin2_theta_t));

        float r_parl = ((eta_t * cos_theta_i) - (eta_i * cos_theta_t)) /
                       ((eta_t * cos_theta_i) + (eta_i * cos_theta_t));
        float r_perp = ((eta_i * cos_theta_i) - (eta_t * cos_theta_t)) /
                       ((eta_i * cos_theta_i) + (eta_t * cos_theta_t));
        return (r_parl * r_parl + r_perp * r_perp) / 2.f;

    }

    // --------------------------------------
    // Cosin weighted BRDF
    // brdf = cos<norm, light> * dw
    // --------------------------------------
    AVec3 CosinWeightedBRDF::evaluateBxDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                          const ALICE_TRACER::Ray &out) {
        // we take the cos<> term as the pdf, so that cos<> term in the render equation is cancelled.
        if(hit_res.mtl_->type() & TextureMaterial){
            auto t_mtl = dynamic_cast<DiffuseMaterial *>(hit_res.mtl_);
            if(t_mtl){
                ImageBase* img = t_mtl->albedoTexture();
                AVec3 albedo = TextureSampler::accessTexture2D(img, hit_res.tex_coord_);// AVec3(hit_res.tex_coord_, 0.f); //MaterialSampler::sampleRGB(img, hit_res.tex_coord_);
                return albedo * AVec3(M_1_PI) * AClamp(ADot(out.dir_, hit_res.normal_));
            }
        }
        return hit_res.mtl_->albedo().ToVec3() * AVec3(M_1_PI) * AClamp(ADot(out.dir_, hit_res.normal_));
    }

    void CosinWeightedBRDF::sampleBxDF(ALICE_TRACER::Ray &out, float &pdf, const ALICE_TRACER::HitRes &hit_res,
                                       const ALICE_TRACER::Ray &in) {
        AVec3 normal = hit_res.normal_;
        // sample Lambert BRDF = albedo/PI * cos<normal, out>
        // so that we can take the cos<> term as the pdf for sampling
        float phi = 2.f * (float)M_PI * random_val<float>();
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
        out.start_ = hit_res.point_;
        out.time_ = 0.f;
        out.fm_t_ = hit_res.frame_time_;
        out.color_ = AVec3(0.f);
        out.dir_ = tangent * x + bitangent * y + normal * z;
        out.ray_t_ = LambertRay;
        pdf = AClamp(ADot(out.dir_, normal)) * (float)M_1_PI;
    }

    float CosinWeightedBRDF::samplePDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                       const ALICE_TRACER::Ray &out) {
        // given a direction, return the pdf
        // Lambert BRDF PDF: cos<dir, normal> / PI
        return AClamp(ADot(out.dir_, hit_res.normal_)) * (float)M_1_PI;
    }

    // --------------------------------------
    // Perfect Mirrored BRDF
    // --------------------------------------
    AVec3 PerfectMirroredBRDF::evaluateBxDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                            const ALICE_TRACER::Ray &out) {
        AVec3 normal = hit_res.normal_;
        AVec3 in_dir = ANormalize(in.dir_);
        AVec3 out_dir = ANormalize(out.dir_);

        AVec3 refl = ANormalize(2.f * AClamp(ADot(-in_dir, normal)) * normal + in_dir);
        if( abs(ADot(out_dir, refl) - 1.f) < MIN_THRESHOLD){  // it should be same as the reflection
            return hit_res.mtl_->albedo().ToVec3() * AClamp(ADot(out_dir, normal));  // it is not physically correct
        }
        return AVec3{0.f};
    }

    float PerfectMirroredBRDF::samplePDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                         const ALICE_TRACER::Ray &out) {
        return 0.f;  // set it to zero
    }

    void PerfectMirroredBRDF::sampleBxDF(ALICE_TRACER::Ray &out, float &pdf, const ALICE_TRACER::HitRes &hit_res,
                                         const ALICE_TRACER::Ray &in) {
        AVec3 in_dir = ANormalize(in.dir_);
        out.start_ = hit_res.point_;
        out.time_ = 0.f;
        out.fm_t_ = hit_res.frame_time_;
        out.color_ = AVec3(0.f);
        out.dir_ = ANormalize(2.f * AClamp(ADot(-in_dir, hit_res.normal_)) * hit_res.normal_ + in_dir);
        out.ray_t_ = SpecularReflectedRay;
        pdf = 1.f;
    }

    // --------------------------------------
    // Perfect Refracted BRDF
    // --------------------------------------
    AVec3 PerfectRefractedBRDF::evaluateBxDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                             const ALICE_TRACER::Ray &out) {
        FresnelMaterial * t_mtl = dynamic_cast<FresnelMaterial *>(hit_res.mtl_);
        if(t_mtl) {
            float F = FresnelDielectric::FrDielectric(hit_res, in, out);
            // assign the value
            AVec3 normal = hit_res.normal_;
            AVec3 in_dir = ANormalize(in.dir_);
            AVec3 out_dir = ANormalize(out.dir_);
            float eta = hit_res.is_inside_ ? t_mtl->eta1()/t_mtl->eta0() : t_mtl->eta0()/t_mtl->eta1();

            // compute the refracted ray
            float cos_theta_i = ADot(in_dir, normal);
            float sin2_theta_i = std::max(0.f, 1.f - cos_theta_i * cos_theta_i);
            float sin2_theta_t = eta * eta * sin2_theta_i;
            float cos_theta_t = std::sqrt(1.f - sin2_theta_t);
            if(sin2_theta_t > 1.f)  // there is no refraction
                return AVec3(0.f);
            AVec3 refr = ANormalize(in_dir * eta - (eta * cos_theta_i + cos_theta_t) * normal);

            if (abs(ADot(out_dir, refr) - 1.f) < MIN_THRESHOLD) {  // it should be same as the reflection
                return hit_res.mtl_->albedo().ToVec3() * (1.f - F);
            }
        }
        return AVec3{0.f};
    }

    float PerfectRefractedBRDF::samplePDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                          const ALICE_TRACER::Ray &out) {
        return 0.f; // set it to zero
    }

    void PerfectRefractedBRDF::sampleBxDF(ALICE_TRACER::Ray &out, float &pdf, const ALICE_TRACER::HitRes &hit_res,
                                          const ALICE_TRACER::Ray &in) {
        FresnelMaterial * t_mtl = dynamic_cast<FresnelMaterial *>(hit_res.mtl_);
        if(t_mtl) {
            // assign the value
            AVec3 normal = hit_res.normal_;
            AVec3 in_dir = ANormalize(in.dir_);
            float eta = hit_res.is_inside_ ? t_mtl->eta1()/t_mtl->eta0() : t_mtl->eta0()/t_mtl->eta1();

            // compute the refracted ray
            float cos_theta_i = ADot(in_dir, normal);
            float sin2_theta_i = std::max(0.f, 1.f - cos_theta_i * cos_theta_i);
            float sin2_theta_t = eta * eta * sin2_theta_i;
            float cos_theta_t = std::sqrt(1.f - sin2_theta_t);
            if(sin2_theta_t > 1.f) { // there is no refraction
                pdf = 0.f;
                return;
            }
            out.dir_ = ANormalize(in_dir * eta - (eta * cos_theta_i + cos_theta_t) * normal);
            out.start_ = hit_res.point_;
            out.time_ = 0.f;
            out.fm_t_ = hit_res.frame_time_;
            out.color_ = AVec3(0.f);
            out.ray_t_ = SpecularRefractedRay;
            pdf = 1.f;
        }
        else{
            assert("Transparent Material is not well defined!\n");
        }
    }

    // --------------------------------------
    // Specular Refracted/Mirrored BxDF
    // --------------------------------------
    AVec3 DielectricSpecularBSDF::evaluateBxDF(const HitRes &hit_res, const Ray &in, const Ray &out) {
        float F = FresnelDielectric::FrDielectric(hit_res, in, out);
        switch (out.ray_t_) {
            case SpecularReflectedRay:
                return F * hit_res.mtl_->albedo().ToVec3();
            case SpecularRefractedRay:
                return (1.f - F) * hit_res.mtl_->albedo().ToVec3();
            default:
                break;
        }
        return AVec3(0.f);
    }

    float DielectricSpecularBSDF::samplePDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                            const ALICE_TRACER::Ray &out) {
        return 0.f;
    }

    void DielectricSpecularBSDF::sampleBxDF(ALICE_TRACER::Ray &out, float &pdf, const ALICE_TRACER::HitRes &hit_res,
                                            const ALICE_TRACER::Ray &in) {
        FresnelMaterial * t_mtl = dynamic_cast<FresnelMaterial *>(hit_res.mtl_);
        if(!t_mtl) {
            assert("material setting is wrong!!\n");
            pdf = 0.f;
            return;
        }

        AVec3 normal = ANormalize(hit_res.normal_);
        AVec3 in_dir = ANormalize(in.dir_);
        float eta = hit_res.is_inside_ ? t_mtl->eta1()/t_mtl->eta0() : t_mtl->eta0()/t_mtl->eta1();

        float F = FresnelDielectric::FrDielectric(hit_res, in, out);
        float u = random_val<float>();
        if(u < F){
            // do the reflection
            out.dir_ = ANormalize(2.f * AClamp(ADot(-in_dir, normal)) * normal + in_dir);
            out.ray_t_ = SpecularReflectedRay;
            pdf = F;
        }
        else{
            // do the refraction
            // compute the refracted ray
            float cos_theta_i = ADot(in_dir, normal);
            float sin2_theta_i = std::max(0.f, 1.f - cos_theta_i * cos_theta_i);
            float sin2_theta_t = eta * eta * sin2_theta_i;
            float cos_theta_t = std::sqrt(1.f - sin2_theta_t);
            out.dir_ = ANormalize(in_dir * eta - (eta * cos_theta_i + cos_theta_t) * normal);
            out.ray_t_ = SpecularRefractedRay;
            pdf = 1.f - F;
        }

        out.start_ = hit_res.point_;
        out.time_ = 0.f;
        out.fm_t_ = hit_res.frame_time_;
        out.color_ = AVec3(0.f);
    }

    // --------------------------------------
    // Microfacet Model
    // Normal Distribution Function
    // --------------------------------------
    float MicrofacetGGX::remapRoughness(float roughness) {
        return roughness * roughness;
    }

    float MicrofacetGGX::isotropicGTR(AVec3 in, AVec3 out, AVec3 normal, float alpha, float omega){
        AVec3 half = ANormalize(out + in);
        float alpha2 = alpha * alpha;
        float cos_theta = AClamp(ADot(half, normal));
        float denom = (cos_theta * cos_theta * (alpha2 - 1.f) + 1.f);
        return alpha2 * (float) M_1_PI / std::pow(denom, omega);
    }

    float MicrofacetGGX::isotropicGGX(AVec3 in, AVec3 out, AVec3 normal, float alpha){
        return isotropicGTR(in, out, normal, alpha, 2.f);
    }

    void MicrofacetGGX::sampleGGX(ALICE_TRACER::Ray &out, const ALICE_TRACER::HitRes &hit_res,
                   const ALICE_TRACER::Ray &in) {
        MetalMaterial * t_mtl = dynamic_cast<MetalMaterial *>(hit_res.mtl_);
        if(!t_mtl) {
            assert("Can not sample GGX!\n");
            return;
        }

        AVec3 normal = ANormalize(hit_res.normal_);
        AVec3 in_dir = -ANormalize(in.dir_);

        // sample Lambert BRDF = albedo/PI * cos<normal, out>
        // so that we can take the cos<> term as the pdf for sampling
        float alpha = t_mtl->roughness();
        float alpha2 = alpha * alpha;
        float phi = 2.f * (float)M_PI * random_val<float>();
        float v = random_val<float>();
        float cos_theta = std::sqrt((1.f - v) / ((alpha2 - 1.f) * v + 1.f));
        float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
        float x = sin_theta * std::cos(phi);
        float y = sin_theta * std::sin(phi);
        float z = cos_theta;

        // transform the outward vector from local coordinate to global coordinate
        AVec3 up;
        abs(normal.z) > 0.9999f ? up = AVec3(1.f, 0.f, 0.f) : up = AVec3(0.f, 0.f, 1.f);
        AVec3 tangent = ANormalize(ACross(up, normal));
        AVec3 bitangent = ACross(normal, tangent);

        AVec3 sample_h = tangent * x + bitangent * y + normal * z;

        // return the sample direction and the pdf
        out.start_ = hit_res.point_;
        out.time_ = 0.f;
        out.fm_t_ = hit_res.frame_time_;
        out.color_ = AVec3(0.f);
        out.dir_ = ANormalize(2.f * AClamp(ADot(in_dir, sample_h)) * sample_h - in_dir);
        out.ray_t_ = GlossyRay;
    }

    // --------------------------------------
    // Mask/Shadowing Term
    // --------------------------------------
    // Height Corrected Smith Mask/Shadowing Function
    float MicrofacetGGX::HeightCorrectedSmith(float ndotv, float ndotl, float r){
        float r2 = r * r;
        float GGX_L = ndotv * std::sqrt((-ndotl * r2 + ndotl) * ndotl + r2);
        float GGX_V = ndotl * std::sqrt((-ndotv * r2 + ndotv) * ndotv + r2);
        return 2.f * ndotv * ndotl / (GGX_L + GGX_V);
    }


    // --------------------------------------
    // Metal BxDF
    // --------------------------------------

    AVec3 MetalBRDF::evaluateBxDF(const HitRes &hit_res, const Ray &in, const Ray &out) {
        MetalMaterial * mtl = dynamic_cast<MetalMaterial *>(hit_res.mtl_);
        if(!mtl) return AVec3(0.f);

        AVec3 normal = ANormalize(hit_res.normal_);
        AVec3 in_dir = -ANormalize(in.dir_);
        AVec3 out_dir = ANormalize(out.dir_);
        AVec3 half = ANormalize(in_dir + out_dir);
        float roughness = mtl->roughness();
        AVec3 f0 = mtl->reflectance();

        float ndoti = AClamp(ADot(normal, in_dir));
        float ndoto = AClamp(ADot(normal, out_dir));
        float idoth = AClamp(ADot(half, in_dir));
//        float ldoth = AClamp(ADot(half, out_dir));
        if(ndoto < MIN_THRESHOLD && idoth < MIN_THRESHOLD) {
            return AVec3(1.f);
        }
        // remap the roughness
        AVec3 F = FresnelSchlick::FSchlick(f0, AVec3(1.f), idoth);
        float D = MicrofacetGGX::isotropicGGX(in_dir, out_dir, normal, roughness);
        float G = MicrofacetGGX::HeightCorrectedSmith(ndoti, ndoto, roughness);

        // bxdf = [F * D * G * / (4 * <n, i> * <n, o>)] * <n, o>
        AVec3 bxdf = F * D * G / (4.f * ndoti);
        return bxdf;

    }

    float MetalBRDF::samplePDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                            const ALICE_TRACER::Ray &out) {
        MetalMaterial * mtl = dynamic_cast<MetalMaterial *>(hit_res.mtl_);
        if(!mtl)
            return 0.f;

        AVec3 normal = ANormalize(hit_res.normal_);
        AVec3 in_dir = -ANormalize(in.dir_);
        AVec3 out_dir = ANormalize(out.dir_);
        AVec3 half = ANormalize(in_dir + out_dir);
        float ndoti = AClamp(ADot(normal, in_dir));
        float ndoto = AClamp(ADot(normal, out_dir));
        float ndoth = AClamp(ADot(normal, half));
        float idoth = AClamp(ADot(half, in_dir));
        float alpha = mtl->roughness();

        if(idoth > MIN_THRESHOLD) {
            float pdf_h = MicrofacetGGX::isotropicGGX(in_dir, out_dir, normal, alpha);
            // pdf = D * ndoth / (4 * vdoth)
            float pdf = pdf_h * ndoth / 4.f / idoth;
            return pdf;
        }
        return MIN_THRESHOLD;
    }

    void MetalBRDF::sampleBxDF(ALICE_TRACER::Ray &out, float &pdf, const ALICE_TRACER::HitRes &hit_res,
                                            const ALICE_TRACER::Ray &in) {
        // sample GGX
        MicrofacetGGX::sampleGGX(out, hit_res, in);
        pdf = samplePDF(hit_res, in, out);
    }


    // --------------------------------------
    // Diffuse BxDF
    // --------------------------------------
    float DisneyDiffuseBRDF::Fresenl90(float roughness, float hdoto) {
        return 0.5f + 2.f * roughness * pow(hdoto, 2.f);
    }

    float DisneyDiffuseBRDF::modifiedFresenl(float roughness, float ndotx, float hdoto) {
        return 1.f + (Fresenl90(roughness, hdoto) - 1.f) * pow((1.f -ndotx), 5.f);
    }

    AVec3 DisneyDiffuseBRDF::evaluateBxDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                   const ALICE_TRACER::Ray &out) {
        AVec3 normal = ANormalize(hit_res.normal_);
        AVec3 in_dir = -ANormalize(in.dir_);
        AVec3 out_dir = ANormalize(out.dir_);
        AVec3 half = ANormalize(in_dir + out_dir);

        float ndoti = AClamp(ADot(normal, in_dir));
        float ndoto = AClamp(ADot(normal, out_dir));
        float hdoto = AClamp(ADot(normal, out_dir));

        if(hit_res.mtl_->type() & TextureMaterial){
            auto t_mtl = dynamic_cast<RoughDiffuseMaterial *>(hit_res.mtl_);
            if(t_mtl){
                // access albedo
                ImageBase* img = t_mtl->albedoTexture();
                AVec3 albedo;
                if(img) {
                    albedo = TextureSampler::accessTexture2D(img,
                                                             hit_res.tex_coord_);// AVec3(hit_res.tex_coord_, 0.f); //MaterialSampler::sampleRGB(img, hit_res.tex_coord_);
                }
                else{
                    albedo = t_mtl->albedo().ToVec3();
                }
                    // access roughness
                ImageBase * rough = t_mtl->roughnessTexture();
                float alpha;
                if(rough){
                    AVec3 value = TextureSampler::accessTexture2D(rough, hit_res.tex_coord_);
                    alpha = value.x;
                }
                else{
                    alpha = t_mtl->roughness();
                }
                float f_l = modifiedFresenl(alpha, ndoto, hdoto);
                float f_v = modifiedFresenl(alpha, ndoti, hdoto);

                return albedo * AVec3(M_1_PI) * f_l * f_v * ndoto;
            }
        }
        return AVec3(0.f);
    }

    float DisneyDiffuseBRDF::samplePDF(const ALICE_TRACER::HitRes &hit_res, const ALICE_TRACER::Ray &in,
                                const ALICE_TRACER::Ray &out) {
        return AClamp(ADot(out.dir_, hit_res.normal_)) * (float)M_1_PI;
    }

    void DisneyDiffuseBRDF::sampleBxDF(ALICE_TRACER::Ray &out, float &pdf, const ALICE_TRACER::HitRes &hit_res,
                                const ALICE_TRACER::Ray &in) {
        AVec3 normal = hit_res.normal_;
        // sample Lambert BRDF = albedo/PI * cos<normal, out>
        // so that we can take the cos<> term as the pdf for sampling
        float phi = 2.f * (float)M_PI * random_val<float>();
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
        out.start_ = hit_res.point_;
        out.time_ = 0.f;
        out.fm_t_ = hit_res.frame_time_;
        out.color_ = AVec3(0.f);
        out.dir_ = tangent * x + bitangent * y + normal * z;
        out.ray_t_ = LambertRay;
        pdf = AClamp(ADot(out.dir_, normal)) * (float)M_1_PI;
    }
}