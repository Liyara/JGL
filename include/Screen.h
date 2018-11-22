#ifndef JGL_SCREEN_H
#define JGL_SCREEN_H

#include "Renderable.h"
#include "color.hpp"
#include "LightSource.h"
#include "Transformable.h"
#include "Mesh.h"

namespace jgl {

    class Texture;

    class Screen : public Renderable, public Translatable, public Scalable {
    public:

        Screen(const Position&, const Dimensions&);
        virtual void render() override;

        virtual void setClearColor(const Color&);
        virtual void clear();
        virtual void setCameraPosition(const Position&);
        virtual void moveCamera(const Position&);
        virtual const Position &getCameraPosition() const;

        virtual int lighting() const;
        virtual void setLightingMode(int);
        virtual void useLightSource(const LightSource&);
        virtual jutil::Queue<LightSource> getLightsInScene() const;

        Texture asTexture() const;

        virtual ~Screen();
    protected:
        uint32_t buffer, bTexture, vbo;
        Color clearColor;
        jutil::Queue<float> vertices;
        friend class Object;
        Position cameraPosition;
        int lightingMode;
        jutil::Queue<LightSource> lights;
        Screen();
    private:

    };

    void initializeScreens();
}

#endif // JGL_SCREEN_H
