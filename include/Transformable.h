#ifndef JGL_TRANSFORMABLE_H
#define JGL_TRANSFORMABLE_H

#include "Translatable.h"
#include "Scalable.h"
#include "Rotatable.h"

namespace jgl {
    class Transformable : public Translatable, public Scalable, public Rotatable {
    public:
        Transformable(const Position &p, const Dimensions &d, const jml::Angle &r) : Translatable(p), Scalable(d), Rotatable(r) {}
    };
}

#endif // JGL_TRANSFORMABLE_H
