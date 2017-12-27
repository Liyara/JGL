
#include "jgl.h"

namespace jgl {
    Quad::Quad(Position p, Dimensions d, Color c) : Object(p, d, c) {
        formShape();
    }

    jutil::Queue<jutil::Queue<float> > Quad::genVAO() {

        jutil::Queue<jutil::Queue<float> > polygon = {
            {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
            {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f}

        };

        return polygon;
    }

    float Quad::area() {
        return (float)size.x() * (float)size.y();
    }
}
