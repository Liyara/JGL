#include "Rotatable.h"

namespace jgl {

    Rotatable::Rotatable(const jml::Angle &r) : rotation(r) {}

    Rotatable &Rotatable::setRotation(const jml::Angle &r) {
        rotation = r;
        return *this;
    }
    Rotatable &Rotatable::rotate(const jml::Angle &r) {
        rotation += r;
        return *this;
    }

    const jml::Angle &Rotatable::getRotation() const {
        return rotation;
    }

    Rotatable::~Rotatable() {}
}
