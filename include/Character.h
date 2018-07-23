#ifndef JGL_CHARACTER_H
#define JGL_CHARACTER_H

#include "Quad.h"
#include "Font.h"

namespace jgl {
    class Character : public Quad, public jutil::NonCopyable {
    public:
        Character(char, const Position&, const Font&, const Color& = Color::White);

        char decode() const;
        Position getOrigin() const;
        Position getBearing() const;
        Position nextPosition() const;
        Font getFont() const;

        virtual ~Character();

    private:
        char raw;
        Position origin, bearing, next;
        Font font;

        void draw();
        friend class Text;
    };
}

#endif // JGL_CHARACTER_H
