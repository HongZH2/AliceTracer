//
// Created by Hong Zhang on 2022/10/29.
//

#ifndef ALICE_TRACER_WINDOW_H
#define ALICE_TRACER_WINDOW_H

// glfw header
#include "third_parties/glfw/include/GLFW/glfw3.h"

namespace ALICE_TRACER{
    // Class Window for interactive window
    class Window {
    public:
        Window() = default;
        ~Window() = default;

        void initWindow(uint32_t w, uint32_t h);
        bool updateWindow();
        void swapBuffer();
        void releaseWindow();
        void getWindowFrameSize(int32_t & width, int32_t & height);
    private:
        GLFWwindow * window_;
        uint32_t width_;
        uint32_t height_;
    };


}

#endif //ALICE_TRACER_WINDOW_H
