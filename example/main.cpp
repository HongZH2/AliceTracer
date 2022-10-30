//
// Created by Hong Zhang on 2022/10/29.
//

#include "interface/include/window.h"
#include "interface/include/imgui_widgets.h"

#include "core/include/image.h"
#include "core/include/camera.h"
#include "core/include/object.h"
#include "core/include/intersection_solver.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_parties/stb_image/stb_write.h"

int main(){
    uint32_t width = 800;
    uint32_t height = 600;
    float w_h_ratio = (float)width/(float)height;

    // create a window
    ALICE_TRACER::Window window{};
    window.initWindow(width, height);
    ALICE_TRACER::ImGUIWidget widgets;
    widgets.initImGui();

    // generate an empty image
    ALICE_TRACER::ImageRGB result_image{width, height};

    // define the camera in the scene
    ALICE_TRACER::Camera camera{
        0.1f, 100.f, ARadians(90.f),
        w_h_ratio, AVec3(0.f, 0.f, 4.f),
        AVec3(0.f, 0.f, 1.f), AVec3(0.f, 1.f, 0.f), AVec3(1.f, 0.f, 0.f)};

    // set up the scene
    ALICE_TRACER::Sphere sphere1{
        AVec3(1.f, 0.f, 0.f), 1.f
    };


    // generate the image pixel by pixel
    for (uint32_t i = 0; i < result_image.h(); ++i) {
        for(uint32_t j = 0; j < result_image.w(); ++j){
            if (j == result_image.h() - 1)
                int s = 1;
            // compute the camera ray
            AVec2i pixel{j, i};
            AVec2i resolution{result_image.w(), result_image.h()};
            ALICE_TRACER::Ray cam_ray = camera.computeRay(pixel, resolution);

            // compute the color
            // ... start path tracing
            float time = ALICE_TRACER::IntersectionSolver::isHitObject(cam_ray, sphere1);
            if(time > 0.f){
                AVec3 point = cam_ray.start_ + time * cam_ray.dir_;
                cam_ray.color_ = sphere1.getNormal(point);
            }
            else{
                cam_ray.color_ = AVec3(0.1f);
            }

            // float to unsigned int 255
            AVec3i color = cam_ray.color_.ToUInt();
            // assign the color to RGB channel
            for (uint32_t c = 0; c < result_image.c(); ++c) {
                result_image(i, j, c) = color[c];
            }
        }
    }

    stbi_write_png("./test.png", result_image.w(), result_image.h(), result_image.c(), result_image.getDataPtr(),0);

    while(window.updateWindow()){
        widgets.updateImGui();
        window.swapBuffer();
    }

    widgets.destroyImGui();
    window.releaseWindow();

    return 0;
}
