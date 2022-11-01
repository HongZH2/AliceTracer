//
// Created by Hong Zhang on 2022/10/29.
//

#include "core/include/camera.h"

namespace ALICE_TRACER{

    // transfer the camera coordinate to the world cooridnate
    void Camera::cameraToWorld(AVec3 & dir) const{
        // rotate the
        AMat3 transform = {forward_, right_, head_up_};
        dir = transform  * dir;
    }

    // Given a pixel and the resolution of the image, compute a single camera ray shot from the center of the pixel
    Ray Camera::getSingleRay(AVec2i pixel, AVec2i resolution, AVec2 offset) const {
        // compute the direction of camera ray
        AVec2 c_pixel = AVec2(pixel) + offset; // center of the current pixel
        float tan_alpha = tan(fov_/2.f);
        AVec3 dir{-1.f, (2. * c_pixel.x/resolution.x - 1.f)* ratio_ * tan_alpha ,(-2.f * c_pixel.y/resolution.y + 1.f) * tan_alpha };
        dir = ANormalize(dir);
        cameraToWorld(dir);
        // time
        float time = 0.f;
        Color color{AVec3(0.f)};
        return {pos_, dir, time, color};
    }

}