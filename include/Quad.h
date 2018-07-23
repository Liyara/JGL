#ifndef QUAD_H
#define QUAD_H

#include "Object.h"

namespace jgl {
    struct Quad : public Object {
    public:
        Quad(const Position&, const Dimensions&, const Color& = Color::White);
        long double area() const;
    protected:
        jutil::Queue<jutil::Queue<long double> > genVAO();
        Quad();
    private:
    };
}

#endif // QUAD_H
