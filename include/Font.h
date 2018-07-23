#ifndef JGL_FONT_H
#define JGL_FONT_H

#include <Core/String.h>

namespace jgl {
    struct Font {
        Font();
        Font(const jutil::String&, unsigned);
        Font(const Font&);
        Font(Font&&);
        Font &operator=(const Font&);
        Font &operator=(Font&&);
        jutil::String file;
        unsigned size;
        bool operator==(const Font&) const;
        bool operator!=(const Font&) const;
    };
}

#endif // JGL_FONT_H
