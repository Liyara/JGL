#include "jgl.h"

namespace jgl {

    Triangle::Triangle(const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c), type(STANDARD) { formShape();}
    Triangle::Triangle(const Position &p, const Dimensions &d, Type t) : Object(p, d, Color::White), type(t) { formShape();}
    Triangle::Triangle(const Position &p, const Dimensions &d, Type t, const Color &c) : Object(p, d, c), type(t) { formShape();}
    Triangle::Triangle(const Position &p, const Dimensions &d, const Color &c, Type t) : Object(p, d, c), type(t) { formShape();}

    jutil::Queue<jml::Vertex> Triangle::generateVertices() const {

        jutil::Queue<jml::Vertex> polygon;

        switch(type) {
            case STANDARD: {
                polygon = {
                    {-1.0f, -1.0f},
                    {1.0f, -1.0f},
                    {0.0f, 1.0}
                };
                break;
            }
            case RIGHT: {
                polygon = {
                    {-1.0f, -1.0f},
                    {1.0f, 1.0f},
                    {1.0f, -1.0f}
                };
                break;
            }
            default: break;
        }

        return polygon;
    }

    long double Triangle::area() const {
        return (1.0 / 2.0) * static_cast<long double>(size.x()) * static_cast<long double>(size.y());
    }
}
