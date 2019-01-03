#include "Quad.h"
#include <iostream>

namespace jgl {

    /*ShaderFile *screenVS, *screenFS;
    Shader *screenShader;*/

    void initializeScreens() {

    }

    Screen::Screen(const Position &p, const Dimensions &d) : Translatable(p), Scalable(d), Resource(FRAME_BUFFER), clearColor(Color::Black), lightingMode(-1), texture(nullptr) {
        generate();
        acquire();
    }

    Screen::Screen() : Translatable(0), Scalable(0), Resource(FRAME_BUFFER), clearColor(Color::Black), lightingMode(-1), texture(nullptr) {}

    Resource &Screen::generate() {
        glGenFramebuffers(1, &_id);
        glBindFramebuffer(GL_FRAMEBUFFER, _id);

        if (texture) delete texture;
        texture = new Image(size, clearColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id(), 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //canvas->build({{-1, 1}, {1, 1}, {1, -1}, {-1, -1}});

        return *this;
    }

    Resource &Screen::destroy() {
        glDeleteFramebuffers(1, &_id);
        return *this;
    }

    void Screen::setClearColor(const Color &c) {
        clearColor = c;
    }

    void Screen::setCameraPosition(const Position &p) {
        cameraPosition = p;
    }

    void Screen::moveCamera(const Position &p) {
        cameraPosition = cameraPosition + p;
    }

    const Position &Screen::getCameraPosition() const {
        return cameraPosition;
    }

    int Screen::lighting() const {
        return lightingMode;
    }

    void Screen::setLightingMode(int m) {
        lightingMode = m;
    }

    void Screen::useLightSource(const LightSource &l) {
        lights.insert(l);
    }

    /*Texture Screen::asTexture() const {
        return *texture;
    }*/

    const Image &Screen::readPixels() const {
        return *texture;
    }

    Screen::~Screen() {
        if (_id) release();
        if (texture) delete texture;
    }

    void Screen::clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, _id);
        auto cN = clearColor.normals();
        glClearColor(cN.r(), cN.g(), cN.b(), cN.a());
        glClear(GL_COLOR_BUFFER_BIT);
        lights.clear();
        if (_id) glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Screen::render() {

    }

    jutil::Queue<LightSource> Screen::getLightsInScene() const {
        return lights;
    }
}
