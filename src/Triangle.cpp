#include "jgl.h"

namespace jgl {

    Triangle::Triangle(Position p, Dimensions d, Color c) : Object(p, d, c), type(STANDARD) {formShape();}
    Triangle::Triangle(Position p, Dimensions d, Type t) : Object(p, d, Color::White), type(t) {formShape();}
    Triangle::Triangle(Position p, Dimensions d, Type t, Color c) : Object(p, d, c), type(t) {formShape();}
    Triangle::Triangle(Position p, Dimensions d, Color c, Type t) : Object(p, d, c), type(t) {formShape();}

    jutil::Queue<jutil::Queue<float> > Triangle::genVAO() {

        jutil::Queue<jutil::Queue<float> > polygon;

        switch(type) {
            case STANDARD: {
                polygon = {
                    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
                    {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
                    {0.0f, 1.0f, 0.0f, 0.5f, 0.0f}
                };
                break;
            }
            case RIGHT: {
                polygon = {
                    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
                    {1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
                    {1.0f, -1.0f, 0.0f, 0.0f, 0.0f}
                };
                break;
            }
            default: break;
        }

        return polygon;
    }

    float Triangle::area() {
        return (1.0f / 2.0f) * (float)size.x() * (float)size.y();
    }
}
