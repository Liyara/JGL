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

    uint32_t operator "" _rgba(unsigned long long);
    uint32_t operator "" _rgab(unsigned long long);
    uint32_t operator "" _ragb(unsigned long long);
    uint32_t operator "" _rabg(unsigned long long);
    uint32_t operator "" _rbga(unsigned long long);
    uint32_t operator "" _rbag(unsigned long long);

    uint32_t operator "" _rgb(unsigned long long);
    uint32_t operator "" _rbg(unsigned long long);
    uint32_t operator "" _rga(unsigned long long);
    uint32_t operator "" _rag(unsigned long long);
    uint32_t operator "" _rba(unsigned long long);
    uint32_t operator "" _rab(unsigned long long);

    uint32_t operator "" _rb(unsigned long long);
    uint32_t operator "" _rg(unsigned long long);
    uint32_t operator "" _ra(unsigned long long);

    uint32_t operator "" _r(unsigned long long);

    uint32_t operator "" _grba(unsigned long long);
    uint32_t operator "" _grab(unsigned long long);
    uint32_t operator "" _garb(unsigned long long);
    uint32_t operator "" _garg(unsigned long long);
    uint32_t operator "" _gbra(unsigned long long);
    uint32_t operator "" _gbar(unsigned long long);

    uint32_t operator "" _bgr(unsigned long long);
    uint32_t operator "" _brg(unsigned long long);
    uint32_t operator "" _bga(unsigned long long);
    uint32_t operator "" _bag(unsigned long long);
    uint32_t operator "" _bra(unsigned long long);
    uint32_t operator "" _bar(unsigned long long);

    uint32_t operator "" _gb(unsigned long long);
    uint32_t operator "" _gr(unsigned long long);
    uint32_t operator "" _ga(unsigned long long);

    uint32_t operator "" _g(unsigned long long);

    uint32_t operator "" _bgra(unsigned long long);
    uint32_t operator "" _bgar(unsigned long long);
    uint32_t operator "" _bagr(unsigned long long);
    uint32_t operator "" _barg(unsigned long long);
    uint32_t operator "" _brga(unsigned long long);
    uint32_t operator "" _brag(unsigned long long);

    uint32_t operator "" _br(unsigned long long);
    uint32_t operator "" _bg(unsigned long long);
    uint32_t operator "" _ba(unsigned long long);

    uint32_t operator "" _b(unsigned long long);

    uint32_t operator "" _grb(unsigned long long);
    uint32_t operator "" _gbr(unsigned long long);
    uint32_t operator "" _gra(unsigned long long);
    uint32_t operator "" _gar(unsigned long long);
    uint32_t operator "" _gba(unsigned long long);
    uint32_t operator "" _gab(unsigned long long);

    uint32_t operator "" _agbr(unsigned long long);
    uint32_t operator "" _agrb(unsigned long long);
    uint32_t operator "" _argb(unsigned long long);
    uint32_t operator "" _arbg(unsigned long long);
    uint32_t operator "" _abgr(unsigned long long);
    uint32_t operator "" _abrg(unsigned long long);

    uint32_t operator "" _agb(unsigned long long);
    uint32_t operator "" _abg(unsigned long long);
    uint32_t operator "" _agr(unsigned long long);
    uint32_t operator "" _arg(unsigned long long);
    uint32_t operator "" _abr(unsigned long long);
    uint32_t operator "" _arb(unsigned long long);

    uint32_t operator "" _ab(unsigned long long);
    uint32_t operator "" _ag(unsigned long long);
    uint32_t operator "" _ar(unsigned long long);

    uint32_t operator "" _a(unsigned long long);
}

#endif //JL_COLOR_H
