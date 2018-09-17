#include "jgl.h"

namespace jgl {
    Poly::Poly(unsigned ps, const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c), points(ps) {
        formShape();
    }


    jutil::Queue<jml::Vertex> Poly::generateVertices() const {
        jutil::Queue<jml::Vertex> poly;
        float angle, xangle = JML_PI / points;
        for (uint16_t i = 0; i < points; ++i) {
            angle = xangle * i * 2;

            jml::Vertex vertex = {
                jml::sin(angle), // X
                jml::cos(angle)  // Y
            };

            poly.insert(vertex);
        }

        return poly;
    }

    long double Poly::area() const {
        auto poly = generateVertices();
        poly.insert(poly[0]);
        long double r = 0;
        for (unsigned i = 0; i < (poly.size() - 1); ++i) {
            const long double &x1_ = poly[i][0], &y1_ = poly[i][1], &x2_ = poly[i + 1][0], &y2_ = poly[i + 1][1];
            long double x1 = x1_ * (getSize().x()) / 2.0;
            long double x2 = x2_ * (getSize().x()) / 2.0;
            long double y1 = y1_ * (getSize().y()) / 2.0;
            long double y2 = y2_ * (getSize().y()) / 2.0;
            long double a = x1 * y2;
            long double b = y1 * x2;
            r += a - b;
        }
        return jml::abs(r / 2.0);
    }
}
