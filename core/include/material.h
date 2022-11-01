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
        Material() = default;
        virtual ~Material() = default;
        Color albedo_;
    };

//    /*
//     * Define the Lambert Material
//     */
//    class LambertDiffuse: public Material{
//    public:
//        explicit LambertDiffuse(Color albedo);
//        ~LambertDiffuse() override;
//        Color albedo_;
//    };

//    /*
//     * Define the Metal Material
//     */
//    class Metal: public Material{
//    public:
//        Metal(Color reflectance, float roughness);
//        ~Metal();
//        float roughness_;
//    };
//
//    /*
//     * Define the Dielectrics
//     */
//    class Dielectric: public Material{
//    public:
//        Dielectric(Color albedo, float roughtness);
//        ~Dielectric();
//    };
}


#endif //ALICE_TRACER_MATERIAL_H
