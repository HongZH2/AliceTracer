//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_CAMERA_H
#define ALICE_TRACER_CAMERA_H

#include ""

namespace ALICE_TRACER{

    class Camera{
    public:
        Camera() = default;
        ~Camera() = default;
    private:
        float near_;
        float far_;
        float fov_;
        float ratio_;

    };

}

#endif //ALICE_TRACER_CAMERA_H
