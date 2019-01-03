#ifndef JGL_SCREEN_H
#define JGL_SCREEN_H

#include "Renderable.h"
#include "color.hpp"
#include "LightSource.h"
#include "Transformable.h"
#include "Mesh.h"

namespace jgl {

    class Texture;
    class Image;

    class Screen : public Renderable, public Translatable, public Scalable, public Resource {
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

        virtual const Image &readPixels() const;

        virtual ~Screen();
    protected:
        Color clearColor;
        Position cameraPosition;
        int lightingMode;
        jutil::Queue<LightSource> lights;
        Mesh canvas;
        Image *texture;

        Resource &generate() override;
        Resource &destroy() override;

        Screen();

        friend class Object;
    private:

    };

    void initializeScreens();
}

#endif // JGL_SCREEN_H
