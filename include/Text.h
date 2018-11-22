#ifndef JGL_TEXT
#define JGL_TEXT

#include "Font.h"

#define _JGL_TEXT_SEGMENT           (_JGL_TEXTURE_SEGMENT + _JGL_TEXTURE_SEGMENT_LENGTH)
#define _JGL_TEXT_SEGMENT_LENGTH    0x02

namespace jgl {

    class Text : public Quad {
    public:



        Text(const Font*, const jutil::String&, const Position&, const Color& = Color::White);

        jutil::String getString() const;
        const Font *getFont() const;

        using Quad::render;
        void render(const Screen*) override;

        void addFilter(uint8_t);
        void removeFilter(uint8_t);
        bool hasFilter(uint8_t) const;

        /** The following methods will INVALIDATE the text object, requring it to be rebuilt.
          * Please use caution as rebuilding the text object can be expensive, especially for long strings.
        **/
        Text &setFont(const Font*);
        Text &setString(const jutil::String&);

    private:

        ///These methods are UNDEFINED for Text objects.

        const Font *font;
        unsigned char *clearData;
        jutil::String str;
        unsigned line;
        bool valid;
        Texture textImage;
        Dimensions maskSize;
        uint32_t filters;

        void buildTexture();
        Dimensions calcTextureSize();
    };

    void textInit();
    void textDestroy();
}

#endif // JGL_TEXT
