#include "Material.h"

namespace jgl {

    const Material
        Material::Gem (
            Color(0.633f * 255.f, 0.727811f * 255.f, 0.633f * 255.f),
            0.6f
        ),

        Material::Rubber (
            Color(0.7f * 255.f, 0.7f * 255.f, 0.7f * 255.f),
            0.078125f
        ),

        Material::Metal (
            Color(0.508273f * 255.f, 0.508273f * 255.f, 0.508273f * 255.f),
            0.4f
        ),

        Material::Pearl (
            Color(0.296648f * 255.f, 0.296648f * 255.f, 0.296648f * 255.f),
            0.088f
        ),

        Material::Chrome (
            Color(0.774597f * 255.f, 0.774597f * 255.f, 0.774597f * 255.f),
            0.6f
        ),

        Material::Plastic (
            Color(0.7f * 255.f, 0.7f * 255.f, 0.7f * 255.f),
            0.25f
        )
    ;

    Material::Material(const Color &s, float sh) : specular(s), shine(sh) {}
    Material::Material(const Material &m) : specular(m.specular), shine(m.shine) {}
    Material::Material() : Material(Material::Metal) {}
}
