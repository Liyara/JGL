#include "Translatable.h"

namespace jgl {

    Translatable::Translatable(const Position &p) : position(p) {}

    Translatable &Translatable::setPosition(const Position &p) {
        position = p;
        return *this;
    }
    Translatable &Translatable::move(const Position &p) {
        position[0] += p[0];
        position[1] += p[1];
        return *this;
    }

    const Position &Translatable::getPosition() const {
        return position;
    }

    Translatable::~Translatable() {}
}
