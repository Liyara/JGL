#ifndef JGL_TRANSLATABLE_H
#define JGL_TRANSLATABLE_H

#include <Vector.hpp>

namespace jgl {

    typedef jml::Vector2f Position;

    class Translatable {
    public:
        Translatable(const Position&);

        virtual Translatable &setPosition(const Position&);
        virtual Translatable &move(const Position&);

        virtual const Position &getPosition() const;

        virtual ~Translatable();

    protected:
        Position position;
    };
}

#endif // JGL_TRANSLATABLE_H
