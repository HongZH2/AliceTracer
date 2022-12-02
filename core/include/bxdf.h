//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_BXDF_H
#define ALICE_TRACER_BXDF_H

#include "material.h"

namespace ALICE_TRACER{

    class HitRes;
    class Ray;
    // --------------------------------------
    // Fresnel Term
    // --------------------------------------
    class FresnelSchlick{
    public:
        FresnelSchlick() = default;
        ~FresnelSchlick() = default;
        static AVec3 FSchlick(AVec3 f0, AVec3 f90, float vdoth) {
            return f0 + (f90 - f0) * std::pow(1.f - vdoth, 5.f);
        }
    };

    class FresnelDielectric{
    public:
        FresnelDielectric() = default;
        ~FresnelDielectric() = default;
        static float FrDielectric(const HitRes & hit_res, const Ray & in, const Ray & out);
    };

    // TODO： Fresnel term for Conductor

    // --------------------------------------
    // Microfacet Model
    // --------------------------------------
    class MicrofacetBase{
    public:
        MicrofacetBase() = default;
        ~MicrofacetBase() = default;
    };

    class MicrofacetGGX: public MicrofacetBase{
    public:
        MicrofacetGGX() = default;
        ~MicrofacetGGX() = default;
        static float remapRoughness(float roughness);
        static void sampleGGX(Ray &out, const HitRes &hit_res, const Ray &in);
        static float isotropicGTR(AVec3 in, AVec3 out, AVec3 normal, float alpha, float omega);
        static float isotropicGGX(AVec3 in, AVec3 out, AVec3 normal, float alpha);
        static float HeightCorrectedSmith(float ndotv, float ndotl, float r);
    };


    // --------------------------------------
    // BxDF Definition
    // --------------------------------------
    class BxDFBase{
    public:
        BxDFBase() = default;
        virtual ~BxDFBase() = default;
         // evaluate the BxDF
        virtual AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) = 0;
        // sample the BxDF
        virtual void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) = 0;
        // compute the pdf of the sample direction
        virtual float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) = 0;
    };

    // --------------------------------------
    // Cosin weighted BRDF
    // brdf = cos<norm, light> * dw
    // --------------------------------------
    class CosinWeightedBRDF: public BxDFBase{
    public:
        CosinWeightedBRDF() = default;
        ~CosinWeightedBRDF() override = default;
        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    };

    // --------------------------------------
    // Perfect Mirrored BRDF
    // --------------------------------------
    class PerfectMirroredBRDF: public BxDFBase{
    public:
        PerfectMirroredBRDF() = default;
        ~PerfectMirroredBRDF() override = default;
        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    };

    // --------------------------------------
    // Perfect Refracted BRDF
    // --------------------------------------
    class PerfectRefractedBRDF: public BxDFBase{
    public:
        PerfectRefractedBRDF() = default;
        ~PerfectRefractedBRDF() override = default;
        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    };

    // --------------------------------------
    // Specular Refracted/Mirrored BxDF
    // --------------------------------------
    class DielectricSpecularBSDF: public BxDFBase{
    public:
        DielectricSpecularBSDF() = default;
        ~DielectricSpecularBSDF() override = default;
        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    };

    // --------------------------------------
    // Diffuse BxDF
    // --------------------------------------
    class DisneyDiffuseBRDF: public BxDFBase{
    public:
        DisneyDiffuseBRDF() = default;
        ~DisneyDiffuseBRDF() override = default;

        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    protected:
        float modifiedFresenl(float roughness, float ndoti, float ndoto);
        float Fresenl90(float roughness, float ndotx);
    };

    // --------------------------------------
    // Metal BxDF
    // --------------------------------------
    class MetalBRDF: public BxDFBase{
    public:
        MetalBRDF() = default;
        ~MetalBRDF() override = default;
        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    };

    // --------------------------------------
    // scaledBxDF
    // --------------------------------------
    class ScaledBxDF: public  BxDFBase{
    public:
        ScaledBxDF(BxDFBase * bxdf): bxdf_(bxdf){

        }
        ~ScaledBxDF() = default;
        AVec3 evaluateBxDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
        void sampleBxDF(Ray & out, float & pdf, const HitRes & hit_res, const Ray & in) override;
        float samplePDF(const HitRes & hit_res, const Ray & in, const Ray & out) override;
    protected:
        AVec3 scale_ = AVec3(1.f);
        BxDFBase * bxdf_;
    };


}

#endif //ALICE_TRACER_BXDF_H
