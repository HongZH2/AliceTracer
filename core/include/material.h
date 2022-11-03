//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_MATERIAL_H
#define ALICE_TRACER_MATERIAL_H

#include "color.h"

namespace ALICE_TRACER{
    /*
     * Define the material class
     */

    class Material {
    public:
        explicit Material(AVec3 albedo): albedo_(albedo){}
        virtual ~Material() = default;

        virtual Color emit(){ return {0.f};}
        inline Color albedo(){return albedo_;}
    protected:
        Color albedo_;
    };

    class EmitMaterial: public Material{
    public:
        EmitMaterial(AVec3 albedo, AVec3 emit): Material(albedo), emit_(emit){}
        ~EmitMaterial() override = default;

        Color emit() override{ return emit_;};

    protected:
        Color emit_;
    };
}


#endif //ALICE_TRACER_MATERIAL_H
