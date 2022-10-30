//
// Created by Hong Zhang on 2022/10/30.
//

#include "interface/include/window.h"

namespace ALICE_TRACER{

    void Window::initWindow(uint32_t w, uint32_t h){
        width_ = w;
        height_ = h;
        if (!glfwInit()){
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    #if defined(__APPLE__)
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
        window_ = glfwCreateWindow(w, h, "Alice Tracer, Welcome!", NULL, NULL);
        if(window_ == nullptr){
            return;
        }
        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1); // Enable vsync
    }

    bool Window::updateWindow() {
        if(glfwWindowShouldClose(window_))
            return false;
        glfwPollEvents();
        return true;
    }

    void Window::getWindowFrameSize(int32_t &width, int32_t &height) {
        glfwGetFramebufferSize(window_, &width, &height);
    }

    void Window::swapBuffer() {
        glfwSwapBuffers(window_);
    }

    void Window::releaseWindow() {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

}
