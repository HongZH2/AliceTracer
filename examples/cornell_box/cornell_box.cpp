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
#include "core/include/integrator.h"
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
    ALICE_TRACER::ImageUByte result_image{ALICE_TRACER::ImageType::IMG_RGB_UByte, width, height, 3};
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
    // bxdf
    ALICE_TRACER::CosinWeightedBRDF lambert;

    // instances
    ALICE_TRACER::RectangleXY * rect0 = new ALICE_TRACER::RectangleXY{AVec3(0.f, 0.f, -4.f), AVec2(8.f), &mtl3, &lambert};
    ALICE_TRACER::RectangleXY * rect5 = new ALICE_TRACER::RectangleXY{AVec3(0.f, 0.f, 4.f), AVec2(8.f), &mtl3, &lambert};
    ALICE_TRACER::RectangleYZ * rect1 = new ALICE_TRACER::RectangleYZ{AVec3(3.5f, 0.f, 0.f), AVec2(8.f), &mtl1, &lambert};
    ALICE_TRACER::RectangleYZ * rect2 = new ALICE_TRACER::RectangleYZ{AVec3(-3.5f, 0.f, 0.f), AVec2(8.f), &mtl2, &lambert};
    ALICE_TRACER::RectangleXZ * rect3 = new ALICE_TRACER::RectangleXZ{AVec3(0.f, 3.f, 0.f), AVec2(8.f), &mtl3, &lambert};
    ALICE_TRACER::RectangleXZ * rect4 = new ALICE_TRACER::RectangleXZ{AVec3(0.f, -3.f, 0.f), AVec2(8.f), &mtl3, &lambert};

    // light
    ALICE_TRACER::EmitMaterial mtl4{AVec3(1.f), AVec3(10.f)};
    ALICE_TRACER::RectangleXZ * rectL = new ALICE_TRACER::RectangleXZ{AVec3(1.f, 2.98f, -2.f), AVec3(1.f), &mtl4, &lambert};
    ALICE_TRACER::RectangleXZ * rectL2 = new ALICE_TRACER::RectangleXZ{AVec3(-1.f, 2.98f, -2.f), AVec3(1.f), &mtl4, &lambert};

    ALICE_TRACER::MirroredMaterial mtl5{ AVec3(1.f)};
    ALICE_TRACER::PerfectMirroredBRDF mirrored;

    ALICE_TRACER::FresnelSpecularMaterial mtl7{ AVec3(1.f), 1.f, 1.5f};
    ALICE_TRACER::DielectricSpecularBSDF dielectric;


    // gold: AVec3(1.f, 0.782f, 0.344f)
    // Aluminum: AVec3(0.913f, 0.922f, 0.924f)
    // Silver AVec3(0.972f, 0.960f, 0.915f)
    ALICE_TRACER::MetalMaterial mtl8{AVec3(1.f), AVec3(0.913f, 0.922f, 0.924f), 0.1f};
    ALICE_TRACER::MetalBRDF metal;

    ALICE_TRACER::TriangleInstance * t1 = new ALICE_TRACER::TriangleInstance{AVec3(0.f, -2.0f, -2.f),
                                                                              AVec3(2.2f), 0.f,
                                                                               AVec3(1.f, 0.f ,0.f),
                                                                               &mtl8,
                                                                              &metal};
    ALICE_TRACER::ModelLoader::loadModel("../assets/venus_lowpoly.obj", t1);
//    ALICE_TRACER::Sphere * sphere = new ALICE_TRACER::Sphere{AVec3(0.f, -2.f, -2.f), 0.8f, &mtl8, &metal};

    // set up the scene
    ALICE_TRACER::Scene scene;
    scene.addCamera(camera);
    scene.addLight(rectL);
    scene.addLight(rectL2);
    scene.addHittable(rect0);
    scene.addHittable(rect1);
    scene.addHittable(rect2);
    scene.addHittable(rect3);
    scene.addHittable(rect4);
    scene.addHittable(rect5);
//    scene.addHittable(sphere);
    scene.addHittable(t1);
    scene.buildBVH();

    // integrator
//    ALICE_TRACER::UniformIntegrator integrator{5, 50, 3};
    ALICE_TRACER::MISIntegrator integrator{100, 50, 5};
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
                        ALICE_TRACER::Color pixel_col = integrator.render(pixel, resolution, &scene);
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

    // render to the screen
    while(window.updateWindow()){
        // render to the screen
        texture.updateTexture(&result_image);
        texture.drawTexture();
        widgets.updateImGui();
        window.swapBuffer();
    }
    widgets.destroyImGui();
    window.releaseWindow();

    stbi_write_png("../showcases/test.png", result_image.w(), result_image.h(), result_image.c(), result_image.getDataPtr(), 0);
    ALICE_TRACER::AliceLog::submitDebugLog("Completed!\n");

    return 0;
}
