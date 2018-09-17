#include "Line.h"
#include "jgl.h"

namespace jgl {
    Line::Line(const Position &pa, const Position &pb) : Line(pa, pb, Color::White, 1) {

    }
    Line::Line(const Position &pa, const Position &pb, const Color &c) : Line(pa, pb, c, 1) {

    }
    Line::Line(const Position &pa, const Position &pb, unsigned w) : Line(pa, pb, Color::White, w) {

    }
    Line::Line(const Position &pa, const Position &pb, const Color &c, unsigned w) : a(pa), b(pb), thickness(w) {
        color = c;
        drawMode = GL_LINES;
        formShape();
    }
    long double Line::area() const {
        return jml::distance(a, b);
    }

    Line &Line::move(unsigned p, const Position &pos) {
        switch(p) {
            case POINT_A: {
                a = a + pos;
                break;
            }
            case POINT_B: {
                b = b + pos;
                break;
            }
        }
        return *this;
    }

    Line &Line::setPosition(unsigned p, const Position &pos) {
        switch(p) {
            case POINT_A: {
                a = pos;
                break;
            }
            case POINT_B: {
                b = pos;
                break;
            }
        }
        return *this;
    }
    Position Line::getPosition(unsigned p) const {
        switch(p) {
            case POINT_A: {
                return a;
            }
            case POINT_B: {
                return b;
            }
            default: return 0;
        }
    }
    Line &Line::setWidth(unsigned w) {
        thickness = w;
        return *this;
    }
    unsigned Line::getWidth() const {
        return thickness;
    }
    jutil::Queue<jml::Vertex> Line::generateVertices() const {
        /*auto s = jgl::getWindow().getSize();
        float yRatio = (float)s.y() / ((float)s.x() / 2.0f);
        jutil::Queue<jutil::Queue<long double> > r = {
            {(a.x() / s.x()) * 2.0, (-a.y() / s.y()) * yRatio, 0.0, 0.0, 0.0},
            {(b.x() / s.x()) * 2.0, (-b.y() / s.y()) * yRatio, 0.0, 1.0, 1.0}
        };
        return r;*/
    }
    void Line::render() {
        glLineWidth(thickness);
        Object::render();
        glLineWidth(1);
    }
    Object &Line::generateMVP() {

        return *this;
    }
}
