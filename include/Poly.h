#ifndef POLY_H
#define POLY_H

#include "Object.h"

namespace jgl {
    struct Poly : public Object {
    public:
        Poly(unsigned, Position, Dimensions, Color = Color::White);
    protected:
        unsigned points;
        jutil::Queue<jutil::Queue<float> > genVAO();
    };
}

#endif // POLY_H
