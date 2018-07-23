
#include "jgl.h"

namespace jgl {
    Quad::Quad(const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c) {
        this->drawMode = GL_QUADS;
        formShape();
    }

    Quad::Quad() : Object({0, 0}, {0, 0}, Color::White) {}

    jutil::Queue<jutil::Queue<long double> > Quad::genVAO() {

        jutil::Queue<jutil::Queue<long double> > polygon = {
            {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
            {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f}

        };

        return polygon;
    }

    long double Quad::area() const {
        return static_cast<long double>(size.x()) * static_cast<long double>(size.y());
    }
}