#ifndef JGL_FONT_H
#define JGL_FONT_H

#include <Core/String.h>
#include <Container/Map.hpp>
#include <Core/NonCopyable.h>
#include "Character.h"

namespace jgl {
    class Font : jutil::NonCopyable {
    public:
        Font(jutil::String, unsigned);

        jutil::String getFile() const;
        unsigned getSize() const;
        const Character *getCharacter(char) const;

        virtual ~Font();
    private:
        jutil::String file;
        unsigned size;
        jutil::Map<char, Character*> characters;
        FT_Library loader;
    };
}

#endif // JGL_FONT_H
