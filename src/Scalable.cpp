#include "Scalable.h"

namespace jgl {
    Scalable::Scalable(const Dimensions &d) : size(d) {}

    Scalable &Scalable::setSize(const Dimensions &d) {
        size = d;
        return *this;
    }
    Scalable &Scalable::scale(const jml::Vector2f &d) {
        size[0] *= d[0];
        size[1] *= d[1];
        return *this;
    }

    const Dimensions &Scalable::getSize() const {
        return size;
    }

    Scalable::~Scalable() {}
}
