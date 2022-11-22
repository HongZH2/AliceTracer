//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_MATERIAL_H
#define ALICE_TRACER_MATERIAL_H

#include "color.h"

namespace ALICE_TRACER{
    enum class MaterialType{
        Lambert,
        Emit,
        SpecularMirror,
        SpecularTransparent,
        Metal
    };
    // ---------------
    // -- Material -----
    // ---------------
    class Material {
    public:
        explicit Material(AVec3 albedo): albedo_(albedo){}
        virtual ~Material() = default;

        virtual Color emit(){ return AVec3(0.f);}
        inline Color albedo(){return albedo_;}
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
            mat_t_ = MaterialType::Emit;
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
        explicit MirroredMaterial(AVec3 albedo): Material(albedo){
            mat_t_ = MaterialType::SpecularMirror;
        }
        ~MirroredMaterial() override = default;
    };

    // ---------------
    // -- Perfect Transparent Material -----
    // ---------------
    class TransparentMaterial: public Material{
    public:
        explicit TransparentMaterial(AVec3 albedo): Material(albedo){
            mat_t_ = MaterialType::SpecularTransparent;
        }
        ~TransparentMaterial() override = default;
    };

    // ---------------
    // -- Metal Material -----
    // ---------------
    class MetalMaterial: public Material{
    public:
        MetalMaterial(AVec3 albedo, AVec3 reflectance, float roughness, float metallic):
                Material(albedo), reflectance_(reflectance), roughness_(roughness), metallic_(metallic){
            mat_t_ = MaterialType::Metal;
        }
        ~MetalMaterial() override = default;

        inline Color reflectance(){return reflectance_;}
        inline float roughness(){return roughness_;}
        inline float metallic(){return metallic_;}

    protected:
        Color reflectance_;
        float roughness_;
        float metallic_;
    };

}


#endif //ALICE_TRACER_MATERIAL_H
