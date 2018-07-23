#include "Ellipse.h"

namespace jgl {
    Ellipse::Ellipse(const Position &p, const Dimensions &d, const Color &c) : Object(p, d, c) {
        formShape();
    }

    jutil::Queue<jutil::Queue<long double> > Ellipse::genVAO() {
        jutil::Queue<jutil::Queue<long double> > polygon;
        unsigned
            diameter = jml::max(size.x(), size.y()),
            points = jml::max(diameter, 100u)
        ;
        long double
            theta = JML_TAU / static_cast<long double>(points),
            tangential = jml::tan(theta),
            radial = jml::cos(theta),
            r = 1.f,
            x = r,
            y = 0.f
        ;
        for (size_t i = 0; i < points; ++i) {
            polygon.insert({x, y});
            long double tx = -y, ty = x;
            x += tx * tangential;
            y += ty * tangential;
            y *= radial;
            x *= radial;
        }
        return polygon;
    }

    long double Ellipse::area() const {
        return JML_PI * (getRadii().x()) * (getRadii().y());
    }

    jml::Vector2ld Ellipse::getRadii() const {
        return {static_cast<long double>(size.x()) / 2.f, static_cast<long double>(size.y()) / 2.f};
    }
}

