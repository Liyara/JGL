#ifndef JL_COLOR_H
#define JL_COLOR_H

#include <Core/integers.h>
#include <Container/Queue.hpp>
#include <Vector.hpp>

namespace jgl {
    struct Color {
    public:
        Color();
        Color(uint8_t, uint8_t, uint8_t, uint8_t = 0xff);
        Color(const Color&);
        bool operator==(const Color&) const;
        bool operator!=(const Color&) const;
        Color &operator=(const Color&);
        uint8_t red() const;
        uint8_t blue() const;
        uint8_t green() const;
        uint8_t alpha() const;
        jml::Vector<uint8_t, 4> asVector() const;
        jml::Vector<float, 4> normals() const;
        const static Color
            Black,
            White,
            Red,
            Blue,
            Green,
            Magenta,
            Yellow,
            Cyan
        ;
        static Color mixture(Color, Color);
        static Color mixture(jutil::Queue<Color>);
        static float normal(uint8_t);
    private:
        uint32_t raw;
    };
}

#endif //JL_COLOR_H
