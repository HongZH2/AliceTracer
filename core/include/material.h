//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_MATERIAL_H
#define ALICE_TRACER_MATERIAL_H

#include "color.h"
#include "image.h"

namespace ALICE_TRACER{
    enum MaterialType{
        PureMaterial = 1 << 0,
        Lambert =  PureMaterial | (1 << 1),
        Specular = PureMaterial | (1 << 2),
        Glossy = PureMaterial | (1 << 3),
        TextureMaterial = (1 << 6),
        LambertTexture = TextureMaterial | (1 << 7)

    };

    static bool isMatchMtlType(MaterialType t1, MaterialType t2){
        return t1 == t2;
    }

    // ---------------
    // -- Material -----
    // ---------------
    class Material {
    public:
        Material() = default;
        explicit Material(AVec3 albedo): albedo_(albedo){}
        virtual ~Material() = default;

        virtual Color emit(){ return AVec3(0.f);}
        virtual Color albedo(){return albedo_;}
        inline MaterialType type(){return mat_t_;}
    protected:
        MaterialType mat_t_ = MaterialType::Lambert;
        Color albedo_;
    };

    // ---------------
    // -- Emit Material -----
    // ---------------
    class EmitMaterial: public Material{
    public:
        EmitMaterial(AVec3 albedo, AVec3 emit): Material(albedo), emit_(emit){
            mat_t_ = MaterialType::Lambert;
        }
        ~EmitMaterial() override = default;
        Color emit() override{ return emit_;}
    protected:
        Color emit_;
    };

    // ---------------
    // -- Perfect Mirrored Material -----
    // ---------------
    class MirroredMaterial: public Material{
    public:
        explicit MirroredMaterial(AVec3 albedo):
                Material(albedo){
            mat_t_ = MaterialType::Specular;
        }
        ~MirroredMaterial() override = default;
    };

    // ---------------
    // -- Perfect Transparent Material -----
    // ---------------
    class FresnelMaterial: public Material{
    public:
        explicit FresnelMaterial(AVec3 albedo, float eta0, float eta1):
                Material(albedo), eta0_(eta0), eta1_(eta1){
            mat_t_ = MaterialType::Specular;
        }
        ~FresnelMaterial() override = default;
        inline float eta0() const{return eta0_;}
        inline float eta1() const{return eta1_;}
    protected:
        float eta0_, eta1_; // outside, inside
    };


    // ---------------
    // -- Perfect Mirrored/Transparent Material -----
    // ---------------
    class FresnelSpecularMaterial: public FresnelMaterial{
    public:
        explicit FresnelSpecularMaterial(AVec3 albedo, float eta0, float eta1):
                FresnelMaterial(albedo, eta0, eta1){
            mat_t_ = MaterialType::Specular;
        }
        ~FresnelSpecularMaterial() override = default;
    };

    // ---------------
    // -- Metal Material
    // -- Cook Torrance Model
    // ---------------
    class MetalMaterial: public Material{
    public:
        MetalMaterial(AVec3 albedo, AVec3 reflectance, float roughness):
                Material(albedo), reflectance_(reflectance), roughness_(roughness){
            mat_t_ = MaterialType::Glossy;
//            roughness < 0.0001？ roughness
        }
        ~MetalMaterial() override = default;

        inline float roughness(){return roughness_;}
        inline AVec3 reflectance(){return reflectance_;}
    protected:
        AVec3 reflectance_;
        float roughness_;
    };

    // ---------------
    // -- Diffuse Material
    // ---------------
    class DiffuseMaterial : public Material{
    public:
        explicit DiffuseMaterial(ImageBase * albedo): albedo_tex_(albedo){
            mat_t_ = LambertTexture;
        }
        virtual ~DiffuseMaterial() = default;
        inline ImageBase * albedoTexture(){return albedo_tex_;}
    protected:
        ImageBase * albedo_tex_;
    };

}


#endif //ALICE_TRACER_MATERIAL_H
