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

    /*
     * Camera Class: pin-hole model
     * Here, the default camera is set to the pin-hole model
     * It will have a little different in this implement between the ones from other source
     */
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
        virtual Ray getSingleRay(AVec2i pixel, AVec2i resolution, AVec2 offset) const;

    public:
        float near_;
        float far_;
        float fov_;
        float ratio_;  // width/height
        float start_fm_;  // start frame time
        float interval_;  // the interval of the open shutter
        AVec3 pos_;

        // the pose of the camera
        AVec3 forward_ = AVec3(0.f, 0.f, 1.f);
        AVec3 head_up_ = AVec3(0.f, 1.f, 0.f);
        AVec3 right_ = AVec3(1.f, 0.f, 0.f);
    };


    /*
     * Thin-Len Camera with the naive simplification
     * refer: https://raytracing.github.io/books/RayTracingInOneWeekend.html#defocusblur
     */
    class ThinLenCamera: public Camera{
    public:
        ThinLenCamera() = default;
        ~ThinLenCamera() = default;
        virtual Ray getSingleRay(AVec2i pixel, AVec2i resolution, AVec2 offset) const override;
    public:
        float aperture_ = 0.f; // the
    };



}

#endif //ALICE_TRACER_CAMERA_H
