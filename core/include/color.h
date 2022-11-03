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
        Color(float val){
            rgb_ = AVec3(val);
        }
        Color(float val1, float val2, float val3){
            rgb_ = AVec3(val1, val2, val3);
        }
        Color(AVec3 col){
            rgb_ = col;
        }

        // convert the float value [0., 1.] to the unsigned integer [0, 255]
        AVec3i ToUInt() const{
            AVec3 temp = AClamp(rgb_, AVec3(0.f), AVec3(1.f));
            return temp * 255.f;
        }

        AVec3 ToVec3() const {
            return rgb_;
        }

        float & operator[](int32_t i){
            return rgb_[i];
        }

        inline Color & operator=(const Color & c)= default;

        inline  Color & operator=(const AVec3 & col){
            rgb_ = col;
            return *this;
        }

        inline Color operator+(const Color & c) const{
            Color temp;
            temp.rgb_ = c.rgb_ + rgb_;
            return temp;
        }

        inline Color operator-(const Color & c) const{
            Color temp;
            temp.rgb_ = c.rgb_ - rgb_;
            return temp;
        }

        inline Color operator/(const Color & c) const{
            Color temp;
            temp.rgb_ = c.rgb_ / rgb_;
            return temp;
        }

        inline Color operator*(const Color & c) const{
            Color temp;
            temp.rgb_ = c.rgb_ * rgb_;
            return temp;
        }

        inline Color & operator+=(const Color & c){
            rgb_ += c.rgb_;
            return *this;
        }

        inline Color & operator-=(const Color & c){
            rgb_ -= c.rgb_;
            return *this;
        }

        inline Color & operator *=(const Color & c){
            rgb_ *= c.rgb_;
            return *this;
        }

        inline Color & operator/=(const Color & c){
            rgb_ /= c.rgb_;
            return *this;
        }

        inline Color & operator/=(const float & c){
            rgb_ /= c;
            return *this;
        }

    protected:
        AVec3 rgb_; // [0., 1.]
    };
}


#endif //ALICE_TRACER_COLOR_H
