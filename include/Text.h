#ifndef JGL_TEXT
#define JGL_TEXT

#include "Character.h"

namespace jgl {
    class Text : public Object {
    public:
        Text(const Font&, const jutil::String&, const Position&, const Color& = Color::White);
        long double area() const;
        jutil::String getString() const;
        Text &setString(jutil::String);
        Text &setFont(const Font&);
        Text &setFontSize(unsigned);
        Text &setFontFile(const jutil::String&);
        Text &setBreakPoint(long double);
        Font getFont() const;

        virtual ~Text();
    private:
        jutil::Queue<Character*> string;
        jutil::String str;
        uint32_t fontSize;
        long double breakPoint;
        Font font;

        jutil::Queue<jutil::Queue<long double> > genVAO();
        void draw() override;
        Object &generateMVP();
        Object &formShape() override;
        Text &clear();
    };
}

#endif // JGL_TEXT
