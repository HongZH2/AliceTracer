//
// Created by Hong Zhang on 2022/10/29.
//

// interface api
#include "interface/include/window.h"
#include "interface/include/imgui_widgets.h"
#include "interface/include/render_texture.h"

// render core components
#include "core/include/image.h"
#include "core/include/camera.h"
#include "core/include/scene.h"
#include "utils/include/alice_threads.h"

// stb image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_parties/stb_image/stb_write.h"

int main(){
    uint32_t width = 1200u;
    uint32_t height = 800u;
    float w_h_ratio = (float)width/(float)height;

    // create a window
    ALICE_TRACER::Window window{};
    window.initWindow(width, height);
    ALICE_TRACER::ImGUIWidget widgets;
    widgets.initImGui();

    // generate an empty image
    ALICE_TRACER::ImageRGB result_image{width, height};
    AVec2i resolution{result_image.w(), result_image.h()};

    // define the camera in the scene
    ALICE_TRACER::ThinLenCamera camera;
    camera.near_ = 0.1f;
    camera.far_ = 100.f;
    camera.fov_ = ARadians(90.f);
    camera.ratio_ = w_h_ratio;
    camera.pos_ = AVec3(0.f, 0.f, 4.f);
    camera.aperture_ = 0.02f;

    // set up the scene
    ALICE_TRACER::Material mtl1;
    mtl1.albedo_ = AVec3(0.9f, 1.f, 0.f);
    ALICE_TRACER::Material mtl2{};
    mtl2.albedo_ = AVec3(1.f, 0.f, 0.f);
    ALICE_TRACER::Material mtl3;
    mtl3.albedo_ = AVec3(1.f);

    ALICE_TRACER::LambertBRDF lambert;

    ALICE_TRACER::Sphere sphere1{AVec3(0.f, -1.f, 3.f), 0.2f, &mtl1, &lambert};
    ALICE_TRACER::Sphere sphere2{AVec3(-2.f, 0.f, -5.f), 3.f, &mtl2, &lambert};
    ALICE_TRACER::Sphere sphere3{AVec3(1.f, -101.f, 0.f), 100.f, &mtl3, &lambert};

    // set up the scene
    ALICE_TRACER::Scene scene;
    scene.addCamera(camera);
    scene.addHittable(&sphere1);
    scene.addHittable(&sphere2);
    scene.addHittable(&sphere3);

    // generate the image pixel by pixel
    // submit multiple
    uint32_t num_pack = 8;
    uint32_t num_column = ceil(result_image.h()/num_pack);
    for(uint32_t n = 0; n < num_pack; ++n){
        // submit the task
        auto t = std::thread([&](){
            for (uint32_t i = n * num_column; i < (n + 1) * num_column && i < result_image.h(); ++i) {
                for (uint32_t j = 0; j < result_image.w(); ++j) {
                    // get the current pixel and re
                    AVec2i pixel{j, i};
                    ALICE_TRACER::Color pixel_col = scene.computePixel(pixel, resolution);

                    // float to unsigned int 255
                    AVec3i color = pixel_col.ToUInt();

                    // assign the color to RGB channel
                    for (uint32_t c = 0; c < result_image.c(); ++c) {
                        result_image(i, j, c) = color[c];
                    }
                }
            }
        });
        t.join();
    }


    // create a texture
    ALICE_TRACER::TextureBuffer texture;
    // render to the screen
    while(window.updateWindow()){
        if(texture.isUpdate()){
            texture.loadGPUTexture(&result_image);
        }
        texture.drawTexture();
        widgets.updateImGui();
        window.swapBuffer();
    }
    widgets.destroyImGui();
    window.releaseWindow();


    stbi_write_png("../showcases/test.png", result_image.w(), result_image.h(), result_image.c(), result_image.getDataPtr(), 0);

    return 0;
}
