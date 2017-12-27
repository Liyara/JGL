#include "Circle.h"

namespace jgl {
    //Circle::Circle(Position p, float r, Color c) : Poly(100, p, {r * 2.0f, r * 2.0f}, c), radius(r) {}
    Circle::Circle(Position p, unsigned r, Color c) : Object(p, {r * 2, r * 2}, c), radius(r) {
        formShape();
    }

    jutil::Queue<jutil::Queue<float> > Circle::genVAO() {
        jutil::Queue<jutil::Queue<float> > polygon;
        size_t points = jml::max(this->radius, 100u);
        float theta = JML_TAU / points;
        float tangential = jml::tan(theta);
        float radial = jml::cos(theta);
        float r = 1.f;
        float x = r;
        float y = 0;
        for (size_t i = 0; i < points; ++i) {
            polygon.insert({x, y});
            float tx = -y;
            float ty = x;
            x += tx * tangential;
            y += ty * tangential;
            y *= radial;
            x *= radial;
        }
        return polygon;
    }

    float Circle::area() {
        return jml::pow(JML_PI * radius, 2);
    }
}

