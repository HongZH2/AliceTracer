//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_CAMERA_H
#define ALICE_TRACER_CAMERA_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "ray.h"

using namespace ALICE_UTILS;

namespace ALICE_TRACER{
    // Camera Class: pin-hole model
    class Camera{
    public:
        Camera() = default;
        ~Camera() = default;

        // transfer the camera coordinate to the world cooridnate
        void cameraToWorld(AVec3 & dir) const;

        /*
        / Given a pixel and the resolution of the image, compute the camera ray
        / offset for sampling multiple direction within one pixel
        */
        Ray getSingleRay(AVec2i pixel, AVec2i resolution, AVec2 offset) const;

    public:
        float near_;
        float far_;
        float fov_;
        float ratio_;  // width/height
        AVec3 pos_;

        // the pose of the camera
        AVec3 forward_;
        AVec3 head_up_;
        AVec3 right_;
    };



}

#endif //ALICE_TRACER_CAMERA_H
