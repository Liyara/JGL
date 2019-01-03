#include "jgl.h"

namespace jgl {

    Triangle::Triangle(const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c), type(STANDARD) { formShape();}
    Triangle::Triangle(const Position &p, const Dimensions &d, Type t) : Object(p, d, Color::White), type(t) { formShape();}
    Triangle::Triangle(const Position &p, const Dimensions &d, Type t, const Color &c) : Object(p, d, c), type(t) { formShape();}
    Triangle::Triangle(const Position &p, const Dimensions &d, const Color &c, Type t) : Object(p, d, c), type(t) { formShape();}

    jutil::Queue<jml::Vertex> Triangle::generateVertices() const {

        switch(type) {
            case STANDARD: {
                return {
                    {-1.0f, -1.0f},
                    {1.0f, -1.0f},
                    {0.0f, 1.0}
                };
                break;
            }
            case RIGHT: {
                return {
                    {-1.0f, -1.0f},
                    {1.0f, 1.0f},
                    {1.0f, -1.0f}
                };
                break;
            }
            default: break;
        }
    }

    long double Triangle::area() const {
        return (1.0 / 2.0) * static_cast<long double>(size.x()) * static_cast<long double>(size.y());
    }
}
