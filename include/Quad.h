#ifndef QUAD_H
#define QUAD_H

#include "Object.h"

namespace jgl {
    struct Quad : public Object {
    public:
        Quad(const Position&, const Dimensions&, const Color& = Color::White);
        Quad(const Quad&);

        void texTest();

        long double area() const;
    protected:
        jutil::Queue<jml::Vertex> generateVertices() const override;
        Quad();
    private:
    };
}

#endif // QUAD_H
