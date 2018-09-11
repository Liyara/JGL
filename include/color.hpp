#ifndef JL_COLOR_H
#define JL_COLOR_H


#include <Vector.hpp>
#include <Core/StringInterface.hpp>

namespace jgl {
    struct Color : public jutil::StringInterface {
    public:
        Color();
        Color(uint8_t, uint8_t, uint8_t, uint8_t = 0xff);
        Color(const Color&);
        Color(uint32_t);
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
        operator uint32_t() const;
        operator jutil::String() override;
        operator const jutil::String() const override;
    private:
        uint32_t raw;
    };
}

#endif //JL_COLOR_H
