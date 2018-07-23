#ifndef JGL_MATERIAL_H
#define JGL_MATERIAL_H

#include "color.hpp"

namespace jgl {
    struct Material {
        Color specular;
        float shine;
        Material(const Color&, float);
        Material(const Material&);
        Material();

        const static Material
            Gem,
            Rubber,
            Metal,
            Pearl,
            Chrome,
            Plastic
        ;

    };
}

#endif // JGL_MATERIAL_H
