#ifndef JGL_LINE_H
#define JGL_LINE_H

#include "Object.h"

namespace jgl {

    enum {
        POINT_A,
        POINT_B
    };

    class Line : public Object {
    public:

        Line(const Position&, const Position&);
        Line(const Position&, const Position&, const Color&);
        Line(const Position&, const Position&, unsigned);
        Line(const Position&, const Position&, const Color&, unsigned);
        long double area() const override;

        Line &move(unsigned, const Position&);
        Line &setPosition(unsigned, const Position&);
        Position getPosition(unsigned) const;
        Line &setWidth(unsigned);
        unsigned getWidth() const;

        void render() override;

    private:
        jutil::Queue<jml::Vertex> generateVertices() const override;
        Object &generateMVP();
        Position a, b;
        unsigned thickness;

        /** These Object methods are applicable to a line. **/
        using Object::setSize;
        using Object::getSize;
        using Object::setPosition;
        using Object::getPosition;
        using Object::scale;
        using Object::move;
        using Object::getOutline;
        using Object::setOutline;
    };
}

#endif // JGL_LINE_H
