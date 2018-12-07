#ifndef JGL_IMAGE_H
#define JGL_IMAGE_H

#include "Screen.h"
#include "Resource.h"

#define JGL_IMAGE_1D            (jgl::Dimensions{(unsigned)-1, 1})
#define JGL_IMAGE_CURRENT_SIZE  ((unsigned)-2)
#define JGL_NULL_IMAGE          (jgl::WordArray{0})

namespace jgl {

    typedef uint32_t TextureID;

    ///ColorArrays and WordArrays are directly convertible.

    typedef jutil::Queue<uint8_t> ByteArray;
    typedef jutil::Queue<uint32_t> WordArray;
    typedef jutil::Queue<Color> ColorArray;

    WordArray packBytes(const ByteArray&, size_t = 4);
    ByteArray unpackBytes(const WordArray&);

    /// Represents and image which can be loaded with raw pixel data.

    class Image : public Scalable, public Resource {
    public:

        typedef uint64_t Handle;

        // When constructing or setting image data, areas outside the specified data but within the specified size will be 0x00000000.

        Image();
        Image(const Dimensions&, const Color& = 0);
        Image(ByteArray&, const Dimensions& = JGL_IMAGE_1D, size_t = 4);
        Image(WordArray&, const Dimensions& = JGL_IMAGE_1D);
        Image(ColorArray&, const Dimensions& = JGL_IMAGE_1D);
        Image(const Image&);
        Image(Image&&);

        const Image &operator=(Image&&);
        const Image &operator=(const Image&);

        ColorArray getImageData(const Position& = 0, const Dimensions& = JGL_AUTOMATIC_SIZE) const;

        Image &setImageData(ByteArray&, const Dimensions& = JGL_IMAGE_CURRENT_SIZE, size_t = 4);
        Image &setImageData(WordArray&, Dimensions = JGL_IMAGE_CURRENT_SIZE);
        Image &setImageData(ColorArray&, const Dimensions& = JGL_IMAGE_CURRENT_SIZE);

        Image &blit(Image&, const Position& = 0, const Position& = 0, const Dimensions& = JGL_AUTOMATIC_SIZE) const;
        Image &clear(const Color&, const Position& = 0, const Dimensions& = JGL_AUTOMATIC_SIZE);

        Scalable &scale(const jml::Vector2f&) override;
        Scalable &setSize(const Dimensions&) override;

        const Handle &handle();

        virtual ~Image();

    private:
        void createGLImage(WordArray&);
        void updateGLImage(WordArray&);

        Dimensions snapToSize(const Dimensions&) const;
        Position snapToGrid(const Position&) const;
        void fixSize(Dimensions*, const Position&) const;

        void generateHandle();

        Resource &generate() override;
        Resource &destroy() override;

        Handle _handle;

        bool acquired;

        friend class jgl::Texture;
        friend void initImages();

        static ResourceID cpySource;
        static ResourceID cpyDest;

    };

    void initImages();
}

#endif // JGL_IMAGE_H
