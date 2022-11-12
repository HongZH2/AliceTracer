//
// Created by Hong Zhang on 2022/10/29.
//

#include "core/include/camera.h"
#include "core/include/random_variable.h"

namespace ALICE_TRACER{

    // transfer the camera coordinate to the world cooridnate
    void Camera::cameraToWorld(AVec3 & dir){
        // rotate the camera ray
        forward_ = ANormalize(pos_ - look_at_);
        if(ADot(head_up_, forward_) > 1.f - MIN_THRESHOLD){
            head_up_ = AVec3(0.f, 0.f, 1.f);
        }
        right_ = ACross(head_up_, forward_);
        AMat3 transform = {forward_, right_, head_up_};
        dir = transform  * dir;
    }

    // Given a pixel and the resolution of the image, compute a single camera ray shot from the center of the pixel
    Ray Camera::getSingleRay(AVec2i pixel, AVec2i resolution, AVec2 offset) {
        // compute the direction of camera ray
        AVec2 c_pixel = AVec2(pixel) + offset; // center of the current pixel
        AVec2 c_resolution = AVec2(resolution) - 1.f;
        float tan_alpha = tan(fov_/2.f);
        AVec3 dir{-1.f, (2. * c_pixel.x/c_resolution.x - 1.f)* ratio_ * tan_alpha ,(-2.f * c_pixel.y/c_resolution.y + 1.f) * tan_alpha };
        dir = ANormalize(dir);
        cameraToWorld(dir);
        // time
        float time = 0.f;
        Color color{AVec3(0.f)};
        // frame time
        float frame_time = start_fm_ + interval_ * random_val<float>();
        return {pos_, dir, time, color, frame_time};
    }


    Ray ThinLenCamera::getSingleRay(AVec2i pixel, AVec2i resolution, AVec2 offset) {
        float lens_radius = aperture_/2.f;
        AVec2 len_offset = lens_radius * 2.f * offset - 0.5f;
        AVec3 look_from = pos_ + head_up_ * len_offset.y + right_ * len_offset.x;

        // compute the direction of camera ray
        AVec2 c_pixel = AVec2(pixel) - len_offset; // center of the current pixel
        float tan_alpha = tan(fov_/2.f);
        AVec3 dir{-1.f, (2. * c_pixel.x/resolution.x - 1.f)* ratio_ * tan_alpha ,(-2.f * c_pixel.y/resolution.y + 1.f) * tan_alpha };
        dir = ANormalize(dir);
        cameraToWorld(dir);
        // time
        float time = 0.f;
        Color color{AVec3(0.f)};
        // frame time
        float frame_time = start_fm_ + interval_ * random_val<float>();
        return {look_from, dir, time, color, frame_time};
    }

}