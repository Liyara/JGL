#include "Window.h"
#include "jgl.h"

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
        clearColor = Color::Black;
        buffer = 0;
        bTexture = 0;
        vbo = 0;
        cameraPosition = 0;
        lightingMode = -1;
        vertices = {
             -1.f, -1.f, 0.f, 0.f, 0.f,
             1.f, -1.f, 0.f, 1.f, 0.f,
             1.f, 1.f, 0.f, 1.f, 1.f,
             -1.f, 1.f, 0.f, 0.f, 1.f
        };
        position = 0;
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
