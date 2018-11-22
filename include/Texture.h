#ifndef JGL_TEXTURE_H
#define JGL_TEXTURE_H

#include "Image.h"

#define JGL_FIRST_FRAME     (unsigned)-4;
#define JGL_LAST_FRAME      (unsigned)-3;
#define JGL_ALL_FRAMES      (unsigned)-2;
#define JGL_CURRENT_FRAME   (unsigned)-1;

namespace jgl {

    class TextureLayer;

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

    enum AnimationMode {
        DEFAULT,
        PAUSED,
        PLAY,
        LOOP,
        PLAY_REVERSE,
        LOOP_REVERSE,
        PLAY_MIRRORED,
        LOOP_MIRRORED,
        RANDOM
    };

    class Texture : public Scalable {
    public:
        Texture();
        Texture(const Image&);
        Texture(const jutil::String&);
        Texture(const Texture&);
        Texture(const Shader&);
        Texture(const Screen&);
        Texture(Texture&&);

        Texture &operator=(const Texture&);
        Texture &operator=(Texture&&);

        Image &operator*();
        Image *operator->();

        Texture &load(const jutil::String&);
        Texture &load(const Image&);
        Texture &load(const Texture&);
        Texture &load(Texture&&);
        Texture &load(const Shader&);
        Texture &load(const Screen&);

        const ResourceID &id() const;
        const Image::Handle &handle();

        virtual ~Texture();
    private:
        Image rawData;
    };

    size_t supportedTextureUnits();
}

#endif // JGL_TEXTURE_H
