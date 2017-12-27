#include "jgl.h"

namespace jgl {
    Poly::Poly(unsigned ps, Position p, Dimensions d, Color c) : Object(p, d, c), points(ps) {
        formShape();
    }


    jutil::Queue<jutil::Queue<float> > Poly::genVAO() {
        jutil::Queue<jutil::Queue<float> > polygon;
        float angle, xangle = JML_PI / points;
        for (uint16_t i = 0; i < points; ++i) {
            angle = xangle * i * 2;

            jml::Vector<long double, 2> vA = {jml::cos(angle), jml::sin(angle)}, vB = {(vA.x() / 2.0f) + 0.5f, ((vA.y() / 2.0f) + 0.5f) + (vA.y() * -1)};

            long double arrVertex[] = {
                vA.x(), // X
                vA.y(), // Y
                0.0f, // Z
                vB.x(), // Texture X
                vB.y()  // Texture Y
            };

            jutil::Queue<float> vertex;
            for (unsigned j = 0; j < 5; ++j) {
                vertex[j] = (float(arrVertex[j]));
            }

            polygon.insert(vertex);
        }

        return polygon;
    }
}
