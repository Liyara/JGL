#include "Text.h"

using namespace jml::literals;

namespace jgl {

    Text::Text(const Font &f, const jutil::String &s, const Position &p, const Color &col) : str(s), font(f) {
        position = p;
        color = col;
        hasTexture = false;
        formed = false;

        rotation = 0_degs;
        components = 0;
        outline = 0;

        formShape();
    }

    long double Text::area() const {
        long double r = 0;
        for (auto &i: string) {
            r += i->area() + (i->nextPosition().x() * i->nextPosition().y());
        }
        return r;
    }

    jutil::Queue<jutil::Queue<long double> > Text::genVAO() {
        return jutil::Queue<jutil::Queue<long double> >();
    }

    Text &Text::setFont(const Font &f) {
        font = f;
        generateMVP();
        return *this;
    }

    Text &Text::setFontSize(unsigned i) {
        return setFont(Font(font.file, i));
    }
    Text &Text::setFontFile(const jutil::String &s) {
        return setFont(Font(s, font.size));
    }
    Font Text::getFont() const {
        return font;
    }
    Text &Text::setBreakPoint(long double b) {
        breakPoint = b;
        return *this;
    }

    Object &Text::formShape() {
        size = {0, 0};
        clear();
        bool def = true;
        long double top, bottom, error = 0, nx, diff, ndiff, nct, ncb;
        Position cp;
        Character *nc, *prev;
        for (auto it = str.begin(); it != str.end(); ++it) {
            cp = position;
            nc = new Character(*it, 0, font, color);
            size = size + nc->size;
            if (it != str.begin()) {
                prev = string.last();
                nx = prev->position.x() + (prev->size.x() / 2.0) + (nc->size.x() / 2.0);
                diff = prev->next.x() - (prev->size.x() + prev->bearing.x());
                ndiff = diff + nc->bearing.x();
                nx += ndiff;
                cp.x() = nx;
                size.x() += ndiff;
            } else {
                error = nc->size.y();
                cp.x() += nc->size.x() / 2.0;
            }
            cp.y() -= (nc->size.y() / 2.0) - (nc->size.y() - nc->bearing.y()) - error;
            nc->setPosition(cp);

            string.insert(nc);

            nct = nc->position.y() - (nc->size.y() / 2.0);
            ncb = nc->position.y() + (nc->size.y() / 2.0);
            if (nct < top || def) {
                top = nct;
            }
            if (ncb > bottom || def) {
                bottom = ncb;
            }
            def = false;
        }
        size.y() = bottom - top;
        formed = true;
        return *this;
    }

    Object &Text::generateMVP() {
        formShape();
        return *this;
    }

    void Text::draw() {
        for (auto i: string) {
            if (i) {
                i->draw();
            }
        }
    }
    Text::~Text() {
        clear();
    }
    Text &Text::clear() {
        for (auto i: string) {
            if (i) {
                delete i;
                i = nullptr;
            }
        }
        string.clear();
        return *this;
    }
    jutil::String Text::getString() const {
        return str;
    }
    Text &Text::setString(jutil::String s) {
        str = s;
        generateMVP();
        return *this;
    }
}


