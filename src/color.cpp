#include "color.hpp"

namespace jgl {
    const Color
        Color::Black(0x00, 0x00, 0x00),
        Color::White(0xff, 0xff, 0xff),
        Color::Red(0xff, 0x00, 0x00),
        Color::Green(0x00, 0xff, 0x00),
        Color::Blue(0x00, 0x00, 0xff),
        Color::Yellow(0xff, 0xff, 0x00),
        Color::Cyan(0x00, 0xff, 0xff),
        Color::Magenta(0xff, 0x00, 0xff)
    ;
    Color::Color() : Color(0xff, 0xff, 0xff) {}
    Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        raw = 0x00000000 | a | (b << 8) | (g << 16) | (r << 24);

    }
    Color Color::mixture(Color a, Color b) {
        jutil::Queue<float> colors = {
            (float)a.red(), (float)a.green(), (float)a.blue(), (float)a.alpha(),
            (float)b.red(), (float)b.green(), (float)b.blue(), (float)b.alpha()
        }, cA, cB;
        for (uint8_t i = 0; i < 0x08; ++i) {
            colors[i] /= 0xff;
            if (i < 4) {
                cA.insert(colors[i]);
            } else {
                cB.insert(colors[i]);
            }
        }
        colors.clear();
        for (uint8_t i = 0; i < 4; ++i) {
            colors.insert(((cA[i] / 2) + (cB[i] / 2)) * 0xff);
        }
        return Color(colors[0], colors[1], colors[2], colors[3]);
    }
    Color Color::mixture(jutil::Queue<Color> c) {
        Color r(0x00, 0x00, 0x00, 0x00);
        for (auto &i: c) {
            r = mixture(r, i);
        }
        return r;
    }
    uint8_t Color::red() const {
        return (raw & 0xff000000) >> 24;
    }

    uint8_t Color::green() const {
        return (raw & 0x00ff0000) >> 16;
    }

    uint8_t Color::blue() const {
        return (raw & 0x0000ff00) >> 8;
    }

    uint8_t Color::alpha() const {
        return raw & 0x000000ff;
    }
    float Color::normal(uint8_t c) {
        return c / 255.f;
    }
    Color::Color(const Color &c) : raw(c.raw) {}
    bool Color::operator==(const Color &c) const {
        return raw == c.raw;
    }
    bool Color::operator!=(const Color &c) const {
        return raw != c.raw;
    }
    Color &Color::operator=(const Color &c) {
        raw = c.raw;
        return *this;
    }
    jml::Vector<uint8_t, 4> Color::asVector() const {
        return {red(), green(), blue(), alpha()};
    }
    jml::Vector<float, 4> Color::normals() const {
        return {normal(red()), normal(green()), normal(blue()), normal(alpha())};
    }
}
