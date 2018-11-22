#include "Image.h"
#include "Dependencies.h"

namespace jgl {

    Image::Image() : Scalable(0), Resource(ResourceType::TEXTURE), acquired(false) {
        createGLImage(WordArray());
    }

    Image::Image(const Dimensions &d, const Color &c) : Image(WordArray(static_cast<uint32_t>(c), d.x() * d.y()), d) {}

    Image::Image(const ByteArray &arr, const Dimensions &d, size_t ch) : Image(packBytes(arr, ch), d) {}

    Image::Image(const WordArray &arr, const Dimensions &d) : Scalable(d), Resource(ResourceType::TEXTURE), acquired(false) {
        if (d == Dimensions JGL_IMAGE_1D) size.x() = arr.size();
        createGLImage(arr);
    }

    Image::Image(ColorArray &arr, const Dimensions &d) : Image(static_cast<WordArray>(arr), d) {}

    Image::Image(Image &&img) : Scalable(img.size), Resource(img._type, img._id), acquired(false) {
        if (img.acquired) {
            acquire();
            acquired = true;
        }
    }

    Resource &Image::generate() {
        glGenTextures(1, &_id);
        glBindTexture(GL_TEXTURE_2D, _id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        _handle = 0;
        return *this;
    }

    const Image::Handle &Image::handle() {
        if (!_handle) generateHandle();
        return _handle;
    }

    void Image::generateHandle() {
        _handle = glGetTextureHandleARB(_id);
        glMakeTextureHandleResidentARB(_handle);
    }

    Resource &Image::destroy() {
        glDeleteTextures(1, &_id);
        if (_handle) glMakeTextureHandleNonResidentARB(_handle);
        return *this;
    }

    Scalable &Image::scale(const jml::Vector2f &s) {
        auto dat = getImageData();
        Scalable::scale(s);
        updateGLImage(dat);
        return *this;
    }

    Scalable &Image::setSize(const Dimensions &d) {
        auto dat = getImageData();
        Scalable::setSize(d);
        updateGLImage(dat);
        return *this;
    }

    const Image &Image::operator=(Image &&img) {
        if (acquired) release();

        _id = img._id;
        size = img.size;

        if (img.acquired) {
            acquire();
            acquired = true;
        } else acquired = false;

        img.size = 0;
        return *this;
    }

    Image::Image(const Image &img) : Scalable(img.size), Resource(img._type), acquired(false) {
        createGLImage(img.getImageData());
    }

    const Image &Image::operator=(const Image &img) {
        if (acquired) release();

        size = img.size;

        if (img.acquired) createGLImage(img.getImageData());
        else acquired = false;

        return *this;
    }

    uint32_t operator "" _rgba(unsigned long long r) {
        return r;
    }

    ColorArray Image::getImageData(const Position &_p, const Dimensions &_dim) const {
        WordArray data;
        auto dim = snapToSize(_dim);
        auto p = snapToGrid(_p);
        fixSize(&dim, p);
        size_t pixels = dim[0] * dim[1];
        data.reserve(pixels).resize(pixels);
        if (pixels) {
            glGetTextureSubImage(
                _id, 0,
                p.x(), p.y(), 0,
                dim.x(), dim.y(), 1,
                GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                pixels * sizeof(uint32_t), (void*)&(data[0])
            );
        }
        return static_cast<ColorArray>(data);
    }


    Image &Image::setImageData(const ByteArray &arr, const Dimensions &d, size_t ch) {
        return setImageData(packBytes(arr, ch), d);
    }

    Image &Image::setImageData(const WordArray &arr, Dimensions dim) {
        if (dim.x() == JGL_AUTOMATIC_SIZE || dim.x() == JGL_IMAGE_CURRENT_SIZE) dim.x() = size.x();
        if (dim.y() == JGL_AUTOMATIC_SIZE || dim.y() == JGL_IMAGE_CURRENT_SIZE) dim.y() = size.y();
        size = dim;
        updateGLImage(arr);
        return *this;
    }

    Image &Image::setImageData(ColorArray &arr, const Dimensions &d) {
        return setImageData(static_cast<WordArray>(arr), d);
    }

    Image &Image::clear(const Color &c, const Position &_p, const Dimensions &_dim) {
        auto dim = snapToSize(_dim);
        auto p = snapToGrid(_p);
        fixSize(&dim, p);
        uint32_t data = static_cast<uint32_t>(c);
        glClearTexSubImage(
            _id, 0,
            p.x(), p.y(), 0,
            dim.x(), dim.y(), 1,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            &data
        );
        return *this;
    }

    Dimensions Image::snapToSize(const Dimensions &dim) const {
        return {
            (dim[0] == JGL_AUTOMATIC_SIZE || dim[0] == JGL_IMAGE_CURRENT_SIZE? size.x() : (dim[0] > size.x()? size.x() : dim[0])),
            (dim[1] == JGL_AUTOMATIC_SIZE || dim[1] == JGL_IMAGE_CURRENT_SIZE? size.y() : (dim[1] > size.y()? size.y() : dim[1]))
        };
    }

    Position Image::snapToGrid(const Position &p) const {
        return {(p[0] < 0? 0 : (p[0] >= size.x()? size.x() - 1 : p[0])), (p[1] < 0? 0 : (p[1] >= size.y()? size.y() - 1 : p[1]))};
    }

    void Image::fixSize(Dimensions *dim, const Position &p) const {
        if (dim->x() + p.x() >= size.x()) dim->x() = size.x() - p.x();
        if (dim->y() + p.y() >= size.y()) dim->y() = size.y() - p.y();
    }

    WordArray packBytes(const ByteArray &arr, size_t ch) {
        WordArray wArr;
        size_t pixels = arr.size() / static_cast<float>(ch);
        wArr.reserve(pixels);

        size_t dataIndex = 0;

        uint8_t alpha, blue, green, red;

        for (size_t i = 0; i < pixels; ++i) {
            dataIndex = i * ch;

            if (ch > 0) red = arr[dataIndex];
            else red = 0;

            if (ch > 1) green = arr[dataIndex + 1];
            else green = 0;

            if (ch > 2) blue = arr[dataIndex + 2];
            else blue = 0;

            if (ch > 3) alpha = arr[dataIndex + 3];
            else alpha = 0xff;

            wArr.insert(alpha | (blue << 8) | (green << 16) | (red << 24));

        }

        return wArr;
    }

    ByteArray unpackBytes(const WordArray &arr) {
        ByteArray bArr;
        size_t bytes = arr.size() * 4;
        bArr.reserve(bytes);

        for (size_t i = 0; i < bytes; ++i) {

            bArr.insert((arr[i] & 0xff000000) >> 24);
            bArr.insert((arr[i] & 0x00ff0000) >> 16);
            bArr.insert((arr[i] & 0x0000ff00) >> 8);
            bArr.insert(arr[i] & 0x000000ff);
        }

        return bArr;
    }

    Image::~Image() {
        release();
    }

    Image &Image::blit(Image &img, const Position &_posOnSource, const Position &_posOnDest, const Dimensions &_dim) const {
        auto dim = snapToSize(_dim);
        auto posOnSource = snapToGrid(_posOnSource);
        auto posOnDest = img.snapToGrid(_posOnDest);
        fixSize(&dim, posOnSource);
        img.fixSize(&dim, posOnDest);
        glCopyImageSubData(_id, GL_TEXTURE_2D, 0, posOnSource.x(), posOnSource.y(), 0, img._id, GL_TEXTURE_2D, 0, posOnDest.x(), posOnDest.y(), 0, dim.x(), dim.y(), 1);
        return img;
    }

    void Image::createGLImage(const WordArray &arr) {
        generate();
        acquire();
        acquired = true;
        updateGLImage(arr);
    }

    void Image::updateGLImage(WordArray arr) {
        if (acquired) {
            glBindTexture(GL_TEXTURE_2D, _id);
            if (!arr.empty()) {
                arr.reserve((size.x() * size.y()));
                for (size_t i = arr.size(); i < (size.x() * size.y()); ++i) {
                    arr.insert(0);
                }
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &(arr[0]));
            } else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
        } else createGLImage(arr);
    }
}
