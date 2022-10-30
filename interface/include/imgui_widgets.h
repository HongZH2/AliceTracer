//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_IMGUI_WIDGETS_H
#define ALICE_TRACER_IMGUI_WIDGETS_H

#include "third_parties/imgui/imgui.h"
#include "third_parties/imgui/backends/imgui_impl_glfw.h"
#include "third_parties/imgui/backends/imgui_impl_opengl3.h"

#include "render_texture.h"

namespace ALICE_TRACER{

    // draw some imgui widgets
    class ImGUIWidget{
    public:
        ImGUIWidget() = default;
        ~ImGUIWidget() = default;

        void initImGui();
        void updateImGui();
        void destroyImGui();

    private:
        void drawWidgets();
    };

}

#endif //ALICE_TRACER_IMGUI_WIDGETS_H
