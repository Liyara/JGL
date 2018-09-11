#ifndef JGL_TEXTURE_H
#define JGL_TEXTURE_H

#include "Screen.h"

namespace jgl {

    class TextureLayer;

    typedef uint32_t TextureID;

    class Shader;

    enum Filter {
        NONE        = 0X00,
        SHARPEN     = 0x01,
        BLUR        = 0X02,
        CHROMA      = 0X04,
        BICUBIC     = 0X08,
        BILATERAL   = 0X10,
    };

    enum Channel : uint8_t {
        RED     = 0X01,
        GREEN   = 0X02,
        BLUE    = 0X04,
        RGB     = 0x07,
        ALPHA   = 0X08,
        RGBA    = 0x0f
    };

    class Texture : public Scalable {
    public:
        Texture();
        Texture(const jutil::String&);
        Texture(const Color&, const Dimensions&);
        Texture(const Texture&);
        Texture(Texture&&);

        Texture &operator=(const Texture&);
        Texture &operator=(Texture&&);

        Texture &load(const jutil::String&);
        Texture &load(TextureID);
        Texture &load(jutil::Queue<uint32_t>, const Dimensions&);
        Texture &load(const jutil::Queue<uint8_t>&, const Dimensions&, uint8_t = RGBA);
        Texture &load(jutil::Queue<Color>, const Dimensions&);
        Texture &load(const Texture&);
        Texture &load(const Color&, const Dimensions&);
        Texture &load(const Screen&);
        Texture &load(const Renderable*, const Dimensions&);
        Texture &load(const Shader&, const Dimensions&);

        Texture &copy(const Texture&);

        TextureID getID() const;

        Texture &addFilters(uint16_t);
        Texture &removeFilters(uint16_t);
        bool hasFilters(uint16_t) const;

        bool hasChannels(uint8_t) const;

        long double getContrast() const;
        long double getSaturation() const;
        long double getBrightness() const;

        Texture &setContrast(long double);
        Texture &setSaturation(long double);
        Texture &setBrightness(long double);

        Texture &swap(uint8_t, uint8_t);
        Texture &clear(uint8_t = RGBA);

        virtual ~Texture();

    private:

        using Scalable::scale;
        using Scalable::setSize;

        void initialize();
        void update(jutil::Queue<Color>*);

        TextureID id;
        uint8_t channels;
        uint16_t filters;
        long double contrast, saturation, brightness;

        void notifyLayers();

        friend class TextureLayer;

        jutil::Queue<TextureLayer*> containers;

    };
}

#endif // JGL_TEXTURE_H
