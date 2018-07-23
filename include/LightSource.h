#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "color.hpp"
#include <Vector.hpp>

enum {
    JGL_LIGHTING_NONE = -1,
    JGL_LIGHTING_DEFAULT = 1,
    JGL_LIGHTING_REALISTIC
};

namespace jgl {

    struct LightSource {
        jml::Vector2f position;
        float intensity;
        Color color;
        LightSource(const jml::Vector2f& , float = 1, const Color& = Color::White);
        LightSource();
        bool operator==(const LightSource&) const;
        bool operator!=(const LightSource&) const;
    };
}

#endif // LIGHTSOURCE_H
