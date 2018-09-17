#ifndef POLY_H
#define POLY_H

#include "Object.h"

namespace jgl {
    struct Poly : public Object {
    public:
        Poly(unsigned, const Position&, const Dimensions&, const Color& = Color::White);
        long double area() const;
    protected:
        unsigned points;
        jutil::Queue<jml::Vertex> generateVertices() const override;
    };
}

#endif // POLY_H
