//
// Created by Hong Zhang on 2022/10/29.
//

// interface api
#include "interface/include/window.h"
#include "interface/include/imgui_widgets.h"
#include "interface/include/render_texture.h"
#include "interface/include/model_loader.h"
// render core components
#include "core/include/image.h"
#include "core/include/camera.h"
#include "core/include/scene.h"
#include "utils/include/alice_threads.h"
// stb image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_parties/stb_image/stb_write.h"

int main(){

    // set the log level
    ALICE_TRACER::AliceLog::setLogLevel(ALICE_TRACER::AliceLogType::debug);

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
    ALICE_TRACER::Camera camera;
    camera.near_ = 0.1f;
    camera.far_ = 100.f;
    camera.fov_ = ARadians(60.f);
    camera.ratio_ = w_h_ratio;
    camera.pos_ = AVec3(0.f, 0.f, 4.f);
//    camera.aperture_ = 0.1f;
    camera.start_fm_ = 0.f;
    camera.interval_ = 0.2f;

    // set up the scene
    // material
    ALICE_TRACER::Material mtl1{AVec3(0.1f, 0.5f, 0.f)};
    ALICE_TRACER::Material mtl2{AVec3(0.5f, 0.1f, 0.f)};
    ALICE_TRACER::Material mtl3{ AVec3(0.3f)};
    ALICE_TRACER::EmitMaterial mtl4{AVec3(1.f), AVec3(10.f)};

    // movement
//    ALICE_TRACER::LinearMovement mv1;
//    mv1.start_ = 0.f;
//    mv1.end_ = 0.2;
//    mv1.velocity_ = AVec3(0.f, 0.2f, 0.f);

    // bxdf
    ALICE_TRACER::LambertBRDF lambert;
    ALICE_TRACER::Sphere* sphere1 = new ALICE_TRACER::Sphere{AVec3(-1.f, -0.3f, 0.f), 0.5f, &mtl1, &lambert};
    ALICE_TRACER::Sphere * sphere2 = new ALICE_TRACER::Sphere{AVec3(-1.5f, -0.3f, 1.f), 0.5f, &mtl2, &lambert};
    ALICE_TRACER::Sphere * sphere3 = new ALICE_TRACER::Sphere{AVec3(1.f, -0.3f, 0.f), 0.5f, &mtl3, &lambert};

//    ALICE_TRACER::RectangleXY * rect1 = new ALICE_TRACER::RectangleXY{AVec3(0.f, 0.f, 0.f), AVec2(1.f), &mtl2, &lambert};
//    ALICE_TRACER::RectangleXZ * rect2 = new ALICE_TRACER::RectangleXZ{AVec3(0.f, 1.f, 0.f), AVec2(1.f), &mtl2, &lambert};
//    ALICE_TRACER::RectangleYZ * rect3 = new ALICE_TRACER::RectangleYZ{AVec3(-1.f, 0.f, 0.f), AVec2(1.f), &mtl2, &lambert};
    ALICE_TRACER::Box * box1 = new ALICE_TRACER::Box{AVec3(-1.f, -1.f, 0.f), AVec3(1.f), &mtl2, &lambert};

    ALICE_TRACER::TriangleMesh * t1 = new ALICE_TRACER::TriangleMesh{AVec3(0.f, -1.f, 0.f), AVec3(1.f), &mtl1, &lambert};
    ALICE_TRACER::ModelLoader::loadModel("../assets/cornell_box/cornell_box.obj", t1);

    // set up the scene
    ALICE_TRACER::Scene scene{5, 5};
    scene.setBgFunc([](AVec3 & dir, AVec3 & col){
        float t = 0.5f * (dir.y + 1.0f);
        col = (1.0f - t) * AVec3(1.0f, 1.0f, 1.0f) + t * AVec3(0.5f, 0.7f, 1.0f);
    });
    scene.addCamera(camera);
//    scene.addHittable(sphere1);
//    scene.addHittable(sphere3);
//    scene.addHittable(box1);
    scene.addHittable(t1);
    scene.buildBVH();

    // create a texture
    ALICE_TRACER::TextureBuffer texture;
    texture.loadGPUTexture(&result_image);

    // generate the image pixel by pixel
    uint32_t num_pack = 16;
    uint32_t num_col = ceil(result_image.h()/num_pack);
    uint32_t num_row = ceil(result_image.w()/num_pack);
    for(uint32_t n = 0; n < num_pack; ++n){
        // submit the task
        for(uint32_t m = 0; m < num_pack; ++m){
            ThreadPool::submitTask([&](uint32_t cur_n, uint32_t cur_m){
                for (uint32_t i = cur_n * num_col; i < (cur_n + 1) * num_col && i < result_image.h(); ++i) {
                    for (uint32_t j = cur_m * num_row; j < (cur_m + 1) * num_row && j < result_image.w(); ++j) {
                        // get the current pixel and resolution
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
            }, n, m);
        }
    }

    while(window.updateWindow()) {
        // render to the screen
        texture.updateTexture(&result_image);
        texture.drawTexture();
        widgets.updateImGui();
        window.swapBuffer();
    }
    widgets.destroyImGui();
    window.releaseWindow();

//    for(auto & t: threads){
//        t.join();
//    }

    stbi_write_png("../showcases/test.png", result_image.w(), result_image.h(), result_image.c(), result_image.getDataPtr(), 0);
    ALICE_TRACER::AliceLog::submitDebugLog("Completed!\n");

    return 0;
}
