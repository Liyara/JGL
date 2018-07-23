#include "Font.h"

namespace jgl {
    Font::Font() : size(0) {}
    Font::Font(const jutil::String &s, unsigned u) : file(s), size(u) {}
    Font::Font(const Font &f) : file(f.file), size(f.size) {}
    Font::Font(Font &&f) : file(f.file), size(f.size) {
        f.file = jutil::String();
        f.size = 0;
    }
    Font &Font::operator=(const Font &f) {
        file = f.file;
        size = f.size;
        return *this;
    }
    Font &Font::operator=(Font &&f) {
        file = f.file;
        size = f.size;
        f.file = jutil::String();
        f.size = 0;
        return *this;
    }
    bool Font::operator==(const Font &f) const {
        return (file == f.file && size == f.size);
    }
    bool Font::operator!=(const Font &f) const {
        return !((*this) == f);
    }
}
