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

    uint32_t width = 800u;
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
    camera.pos_ = AVec3(0.f, 2.f, 4.f);
    camera.look_at_ = AVec3(0.f, 0.f, 0.f);
    camera.start_fm_ = 0.f;
    camera.interval_ = 0.2f;
    //    camera.aperture_ = 0.1f;

    // set up the scene
    // Rectangle
    ALICE_TRACER::Material mtl1{AVec3(0.02f)};
    ALICE_TRACER::Material mtl2{AVec3(0.3f, 0.3f, 0.f)};
    ALICE_TRACER::Material mtl3{ AVec3(0.35f)};
    ALICE_TRACER::MirroredMaterial mtl4{AVec3(1.0f)};
    ALICE_TRACER::FresnelMaterial mtl5{AVec3(1.0f), 1.f, 1.5f};
    ALICE_TRACER::FresnelSpecularMaterial mtl6{AVec3(1.0f), 1.f, 1.5f};

    // silver AVec3(0.972f, 0.960f, 0.915f)
    // copper AVec3(0.955f, 0.638f, 0.538f)
    // gold AVec3(1.f, 0.782f, 0.344f)
    ALICE_TRACER::MetalMaterial mtl8{AVec3(1.f),  AVec3(0.955f, 0.638f, 0.538f), 0.01f};
    ALICE_TRACER::MetalMaterial mtl11{AVec3(1.f), AVec3(0.955f, 0.638f, 0.538f), 0.5f};
    ALICE_TRACER::RoughDiffuseMaterial mtl13{AVec3(0.2f), 1.f};


    ALICE_TRACER::ImageBase * img = ALICE_TRACER::ImagePool::loadRGB("checker", "../assets/images/default.png"); //colorful_grids.jpg");
    ALICE_TRACER::DiffuseMaterial mtl9{img};

    ALICE_TRACER::CosinWeightedBRDF lambert;
    ALICE_TRACER::PerfectMirroredBRDF mirrored;
    ALICE_TRACER::PerfectRefractedBRDF refracted;
    ALICE_TRACER::DielectricSpecularBSDF dielectric;
    ALICE_TRACER::DisneyDiffuseBRDF disney_diffuse;

    // uber bxdf
    ALICE_TRACER::UberMaterial uber_mtl;
    ALICE_TRACER::RoughDiffuseMaterial mtl12{AVec3(0.8f, 0.2f, 0.18f), 1.0f};
    ALICE_TRACER::MetalMaterial mtl10{AVec3(1.f), AVec3(0.8f, 0.2f, 0.18f), 0.001f};
    uber_mtl.pushLayer(&mtl12);
    uber_mtl.pushLayer(&mtl10);

    ALICE_TRACER::DisneyDiffuseBRDF disney_diffuse2;
    ALICE_TRACER::ScaledBxDF scaled_ddiffuse = {&disney_diffuse2, AVec3(0.25f)};
    ALICE_TRACER::MetalBRDF metal;
    ALICE_TRACER::ScaledBxDF scaled_metal = {&metal, AVec3(0.75f)};
    ALICE_TRACER::UberBxdf uber;
    uber.pushLayer(&scaled_ddiffuse);
    uber.pushLayer(&scaled_metal);

    // instances
    ALICE_TRACER::RectangleXZ * rect0 = new ALICE_TRACER::RectangleXZ{AVec3(0.f, -1.f, 0.f), AVec3(10.f), &mtl9, &lambert};
//    ALICE_TRACER::Sphere * sphere = new ALICE_TRACER::Sphere{AVec3(0.f, 1.f, 0.f), 1.f, &uber_mtl, &uber};


    ALICE_TRACER::TriangleInstance * t2 = new ALICE_TRACER::TriangleInstance{AVec3(0.f, -0.95f, -0.3f),
                                                                             AVec3(0.6f),
                                                                             0.f,
                                                                             AVec3(0.f, 1.f ,0.f),
                                                                             &uber_mtl,
                                                                             &uber};
    ALICE_TRACER::ModelLoader::loadModel("../assets/mitsuba/mitsuba-sphere.obj", t2);
    t2->setTriangleMaterial(0, &mtl13, &disney_diffuse);


    ALICE_TRACER::ImageBase * env_img = ALICE_TRACER::ImagePool::loadHdr("checker", "../assets/images/marry_hall_4k.hdr");
    ALICE_TRACER::EnvironmentalLight * env = new ALICE_TRACER::EnvironmentalLight(env_img);

    // set up the scene
    ALICE_TRACER::Scene scene;
    scene.addCamera(camera);
    scene.addLight(env);
    scene.addHittable(rect0);
    scene.addHittable(t2);
//    scene.addHittable(sphere);
    scene.buildBVH();

    // integrator
    ALICE_TRACER::MISIntegrator integrator{1000, 50, 5};

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

    stbi_write_png("../showcases/material.png", result_image.w(), result_image.h(), result_image.c(), result_image.getDataPtr(), 0);
    ALICE_TRACER::AliceLog::submitDebugLog("Completed!\n");
    return 0;
}
