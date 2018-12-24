
#include "jgl.h"

namespace jgl {
    Quad::Quad(const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c) {
        formShape();
    }

    Quad::Quad() : Object({0, 0}, {0, 0}, Color::White) {}
    Quad::Quad(const Quad &q) : Object(q) {}


    jutil::Queue<jml::Vertex> Quad::generateVertices() const {

        jutil::Queue<jml::Vertex> polygon = {
            {-1, 1},
            {1, 1},
            {1, -1},
            {-1, -1}
        };

        return polygon;
    }

    long double Quad::area() const {
        return static_cast<long double>(size.x()) * static_cast<long double>(size.y());
    }
}
