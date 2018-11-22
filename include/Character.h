#ifndef JGL_CHARACTER_H
#define JGL_CHARACTER_H

#include "Quad.h"

namespace jgl {
    class Character: public jutil::NonCopyable {
    public:
        Character(FT_Library, jutil::String, char, unsigned);

        char decode() const;
        Position getOrigin() const;
        Position getBearing() const;
        Position nextPosition() const;
        const Image &getImage() const;
        Dimensions getSize() const;

        unsigned getFontSize() const;
        jutil::String getFontFile() const;

        void draw();

        virtual ~Character();

    private:
        char raw;
        Position origin, bearing, next;
        Dimensions size;
        jutil::String file;
        unsigned fSize;
        Image image;
    };
}

#endif // JGL_CHARACTER_H
