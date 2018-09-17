
#include "jgl.h"

namespace jgl {
    Quad::Quad(const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c) {
        this->drawMode = GL_QUADS;
        formShape();
    }

    Quad::Quad() : Object({0, 0}, {0, 0}, Color::White) {}



    jutil::Queue<jml::Vertex> Quad::generateVertices() const {

        jutil::Queue<jml::Vertex> polygon = {
            {-1.f, -1.f},
            {1.f, -1.f},
            {1.f, 1.f},
            {-1.f, 1.f}

        };

        return polygon;
    }

    long double Quad::area() const {
        return static_cast<long double>(size.x()) * static_cast<long double>(size.y());
    }
}
