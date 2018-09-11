#ifndef JGL_SCALABLE_H
#define JGL_SCALABLE_H

#include <Vector.hpp>

namespace jgl {

    typedef jml::Vector2u Dimensions;

    class Scalable {
    public:
        Scalable(const Dimensions&);

        virtual Scalable &setSize(const Dimensions&);
        virtual Scalable &scale(const jml::Vector2f&);

        virtual const Dimensions &getSize() const;

        virtual ~Scalable();
    protected:
        Dimensions size;
    };
}

#endif // JGL_SCALABLE_H
