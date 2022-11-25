// -----------------------------------------------------------
// Define Common Math Structure
// Created by Hong Zhang on 2022/10/28.
// -----------------------------------------------------------

#ifndef ALICE_TRACER_ALICE_MATH_H
#define ALICE_TRACER_ALICE_MATH_H
#include "third_parties/glm/glm.hpp"
#include "third_parties/glm/gtc/matrix_transform.hpp"
#include "third_parties/glm/gtc/quaternion.hpp"
#include "third_parties/glm/gtx/matrix_decompose.hpp"
#include "third_parties/glm/gtx/quaternion.hpp"
#include "third_parties/glm/gtx/transform.hpp"
#include "third_parties/glm/gtx/rotate_vector.hpp"
#include "third_parties/glm/gtc/type_ptr.hpp"
#include <cmath>

namespace ALICE_UTILS {
    // vector
    typedef glm::vec3 AVec3;
    typedef glm::vec2 AVec2;
    typedef glm::vec4 AVec4;

    typedef glm::ivec2 AVec2i;
    typedef glm::ivec3 AVec3i;
    typedef glm::ivec4 AVec4i;

    typedef glm::quat AQuat;

    // matrix
    typedef glm::mat3x3 AMat3;
    typedef glm::mat4x4 AMat4;
    typedef glm::mat2x2 AMat2;

    // functions

    template <typename T>
    inline decltype(auto) AIsNan(const T & vec){
        return glm::all(glm::isnan(vec));
    }

    template <typename T>
    inline decltype(auto) APow(const T & vec, const T & index){
        return glm::pow(vec, index);
    }

    inline float AClamp(float val){
        return glm::clamp(val, 0.f, 1.f);
    }

    template <typename T>
    inline decltype(auto) AClamp(const T & vec, const T & min, const T & max){
        return glm::clamp(vec, min, max);
    }

    template <typename T>
    inline decltype(auto) AValuePtr(T & data){
        return glm::value_ptr(data);
    }

    template <typename T>
    inline decltype(auto) AQuatCast(T const& rot_mat){
        return glm::quat_cast(rot_mat);
    }

    inline decltype(auto) AMat4Cast(AQuat const& rot_quat){
        return glm::mat4_cast(rot_quat);
    }

    inline decltype(auto) AMat3Cast( AQuat const& rot_quat){
        return glm::mat3_cast(rot_quat);
    }

    inline decltype(auto) AQuat2Euler(AQuat const& rot_quat){
        return glm::eulerAngles(rot_quat);
    }


    inline bool ADecomposeMat(AMat4 & mat, AVec3 & scale, AQuat & quat, AVec3 & translation, AVec3 & skew, AVec4 & perspective){
        return glm::decompose(mat, scale, quat, translation, skew, perspective);
    }

    template <typename T>
    inline decltype(auto) ADot (T const& vec1, T const& vec2){
        return glm::dot(vec1, vec2);
    }

    template <typename T>
    inline decltype(auto) ADistance(T const& vec1, T const& vec2){
        return glm::distance(vec1, vec2);
    }

    template <typename T>
    inline decltype(auto) ALength(T const& vec1){
        return glm::length(vec1);
    }

    template <typename T>
    inline decltype(auto) ANormalize (T const& vec){
        return glm::normalize(vec);
    }

    template <typename T>
    inline decltype(auto) ACross (T const& vec1, T const& vec2){
        return glm::cross(vec1, vec2);
    }

    template <typename T>
    inline decltype(auto) ARadians (T const& val){
        return glm::radians(val);
    }

    template <typename T>
    inline decltype(auto) ADegrees (T const& val){
        return glm::degrees(val);
    }

    // translation
    template <typename T>
    inline decltype(auto) ATranslate (T const& mat, AVec3 const& vec){
        return glm::translate(mat, vec);
    }

    // scaling
    template <typename T>
    inline decltype(auto) AScale (T const& mat, AVec3 const& vec){
        return glm::scale(mat, vec);
    }

    // translation
    template <typename T>
    inline decltype(auto) ARotate (T const& mat, float angle, AVec3 const& vec){
        return glm::rotate(mat, angle, vec);
    }

    // lookAt transformation
    inline decltype(auto) ALookAt(AVec3 const & pos, AVec3 const& center, AVec3 const& up){
        return glm::lookAt(pos, center, up);
    }

    // perspective transformation
    template <typename T>
    inline decltype(auto) APerspective(T const& fovy, T const& ratio, T const& near, T const& far){
        return glm::perspective(fovy, ratio, near, far);
    }

    // orth
    template <typename T>
    inline decltype(auto) AOrtho(T const& left, T const& right,  T const& top, T const& bottom, T const& near, T const& far){
        return glm::ortho(left, right, top, bottom, near, far);
    }

    // inverse
    template <typename T>
    inline decltype(auto) AInverse(T const & mat){
        return glm::inverse(mat);
    }

    // transpose
    template <typename T>
    inline decltype(auto) ATranspose(T const & mat){
        return glm::transpose(mat);
    }

}


#endif //ALICE_TRACER_ALICE_MATH_H
