//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_COLOR_H
#define ALICE_TRACER_COLOR_H

#include "utils/include/alice_math.h"
using namespace ALICE_UTILS;
namespace ALICE_TRACER{

    class Color{
    public:
        Color() = default;
        ~Color() = default;

        // convert the float value [0., 1.] to the unsigned integer [0, 255]
        AVec3i ToUInt() const{
            AVec3 temp = AClamp(rgb, AVec3(0.f), AVec3(1.f));
            return temp * 255.f;
        }

        float & operator[](int32_t i){
            return rgb[i];
        }

        Color & operator=(const Color & c){
            rgb = c.rgb;
            return *this;
        }

        Color & operator=(const AVec3 & col){
            rgb = col;
            return *this;
        }
    public:
        AVec3 rgb; // [0., 1.]
    };
}


#endif //ALICE_TRACER_COLOR_H
