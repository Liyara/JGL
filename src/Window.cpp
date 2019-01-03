#include "Window.h"
#include "jgl.h"
#include <iostream>

namespace jgl {

    void stepAnimations() {
        auto &animations = Animation::getPlayingAnimations();
        for (auto &i: animations) {
            i->step();
        }
    }

    Window *instance = NULL;

    Window::Window(Dimensions s) {
        if (instance) delete instance;
        instance = this;
        size = s;
    }

    void Window::render() {
        stepAnimations();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0.f, -(((float)size[0] - (float)size[1]) / 2.0f), (float)size[0], (float)size[1] * ((float)size[0] / (float)size[1]));
        glfwSwapBuffers(getWindowHandle());
    }

    Window::~Window() {
        if (instance) instance = NULL;
    }

    Window *const getWindow() {
        return instance;
    }
}
