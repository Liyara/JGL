#include "TextureLayer.h"
#include "Dependencies.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FILL_TEXTURE(_dat_) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &((static_cast<jutil::Queue<uint32_t> >(_dat_))[0]))
#define FILL_TEXTURE_NULL glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);

namespace jgl {

    void Texture::update(jutil::Queue<Color> *data) {
        glBindTexture(GL_TEXTURE_2D, id);
        if (data && !data->empty()) {
            data->reserve((size.x() * size.y()));
            for (size_t i = 0; i < (size.x() * size.y()); ++i) data->insert(0);
            FILL_TEXTURE(*data);
        } else FILL_TEXTURE_NULL;
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture &Texture::load(const Color &c, const Dimensions &d) {
        size = d;
        jutil::Queue<Color> *data = new jutil::Queue<Color>(c, size.x() * size.y());
        update(data);
        delete data;
        return *this;
    }

    Texture &Texture::load(const jutil::String &s) {
        char ss[s.size() + 1];
        s.array(ss);
        int nrChannels;
        jml::Vector2u ssize;
        uint8_t *sData = stbi_load(ss, reinterpret_cast<int*>(&(ssize[0])), reinterpret_cast<int*>(&(ssize[1])), &nrChannels, STBI_rgb_alpha);

        load(jutil::Queue<uint8_t>(sData, (ssize[0] * ssize[1]) * nrChannels), ssize);

        return *this;
    }

    Texture &Texture::load(uint32_t od) {
        glBindTexture(GL_TEXTURE_2D, od);

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, reinterpret_cast<int*>(&(size[0])));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, reinterpret_cast<int*>(&(size[1])));
        uint32_t *_dat = NULL;
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, _dat);

        glBindTexture(GL_TEXTURE_2D, 0);

        load(jutil::Queue<uint32_t>(_dat, size[0] * size[1]), size);
        free(_dat);

        return *this;
    }

    Texture &Texture::load(jutil::Queue<uint32_t> dat, const Dimensions &d) {
        size = d;
        jutil::Queue<Color> data = static_cast<jutil::Queue<Color> >(dat);

        update(&data);

        return *this;
    }

    Texture &Texture::load(jutil::Queue<Color> dat, const Dimensions &d) {
        size = d;

        update(&dat);

        return *this;
    }

    #define __JGL_NEXT_CHANNEL(flags, color, data, cursor, index, default) (flags & color? data[index + cursor++] : default)

    Texture &Texture::load(const jutil::Queue<uint8_t> &sData, const Dimensions &d, uint8_t channels) {
        size = d;

        jutil::Queue<Color> *data = new jutil::Queue<Color>();

        size_t pixels = size[0] * size[1], dataIndex = 0, cursor, nrChannels = 0;

        for (size_t i = RED; i < RGBA; i *= 2) if (channels & i) ++nrChannels;

        uint8_t alpha, blue, green, red;

        data->reserve(pixels);
        for (size_t i = 0; i < pixels; ++i) {
            dataIndex = i * nrChannels;
            cursor = 0;

            red = __JGL_NEXT_CHANNEL(channels, RED, sData, cursor, dataIndex, 0x00);
            green = __JGL_NEXT_CHANNEL(channels, GREEN, sData, cursor, dataIndex, 0x00);
            blue = __JGL_NEXT_CHANNEL(channels, BLUE, sData, cursor, dataIndex, 0x00);
            alpha = __JGL_NEXT_CHANNEL(channels, ALPHA, sData, cursor, dataIndex, 0xff);

            data->insert(alpha | (blue << 8) | (green << 16) | (red << 24));

        }

        update(data);
        delete data;

        return *this;
    }

    Texture &Texture::load(const Renderable *r, const Dimensions &d) {
        /// THINKING...
        return *this;
    }

    Texture &Texture::load(const Shader &s, const Dimensions &d) {
        /// THINKING...
        return *this;
    }

    Texture &Texture::load(const Texture &t) {
        load(t.id);
        return *this;
    }

    void Texture::initialize() {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::Texture() : Scalable(0) {
        initialize();
        FILL_TEXTURE_NULL;
    }

    Texture::Texture(const jutil::String &s) : Scalable(0) {
        initialize();
        load(s);

    }

    Texture::Texture(const Color &c, const Dimensions &d) : Scalable(d) {
        initialize();
        load(c, d);
    }

    Texture &Texture::copy(const Texture &t) {
        Dimensions ns;
        if (t.size[0] * t.size[1] > size[0] * size[1]) ns = size;
        else ns = t.size;
        glCopyImageSubData(t.id, GL_TEXTURE_2D, 0, 0, 0, 0, id, GL_TEXTURE_2D, 0, 0, 0, 0, ns.x(), ns.y(), 1);

        return *this;
    }

    Texture::Texture(const Texture &t) : Scalable(0) {
        initialize();
        *this = t;
    }

    Texture::Texture(Texture &&t) : Scalable(0) {
        initialize();
        *this = jutil::move(t);
    }

    Texture &Texture::operator=(const Texture &t) {
        size = t.size;
        glBindTexture(GL_TEXTURE_2D, id);
        FILL_TEXTURE_NULL;
        glBindTexture(GL_TEXTURE_2D, 0);
        glCopyImageSubData(t.id, GL_TEXTURE_2D, 0, 0, 0, 0, id, GL_TEXTURE_2D, 0, 0, 0, 0, size.x(), size.y(), 1);

        return *this;
    }

    Texture &Texture::operator=(Texture &&t) {
        size = t.size;
        glBindTexture(GL_TEXTURE_2D, id);
        FILL_TEXTURE_NULL;
        glBindTexture(GL_TEXTURE_2D, 0);
        glCopyImageSubData(t.id, GL_TEXTURE_2D, 0, 0, 0, 0, id, GL_TEXTURE_2D, 0, 0, 0, 0, size.x(), size.y(), 1);

        t.size = 0;
        t.id = 0;

        return *this;
    }

    #define __JGL_SWAP_AB_SELECT_CHANNEL(_ch) (a == _ch? b : (b == _ch? a : _ch)),

    Texture &Texture::swap(uint8_t a8, uint8_t b8) {
        GLint a = jml::min((a8 >> 1), 0x03) + GL_RED;
        GLint b = jml::min((b8 >> 1), 0x03) + GL_RED;
        glBindTexture(GL_TEXTURE_2D, id);
        GLint swizzleMask[] = { JUTIL_CALL_UNROLL(__JGL_SWAP_AB_SELECT_CHANNEL, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA) };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        glBindTexture(GL_TEXTURE_2D, 0);

        return *this;
    }

    Texture &Texture::clear(uint8_t c) {
        glBindTexture(GL_TEXTURE_2D, id);
        GLint swizzleMask[] = {!(c & RED) * GL_RED, !(c & GREEN) * GL_GREEN, !(c & BLUE) * GL_BLUE, !(c & ALPHA) * GL_ALPHA};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        glBindTexture(GL_TEXTURE_2D, 0);

        return *this;
    }

    uint32_t Texture::getID() const {
        return id;
    }

    Texture &Texture::addFilters(uint16_t f) {
        filters |= f;
        return *this;
    }

    Texture &Texture::removeFilters(uint16_t f) {
        filters &= ~f;
        return *this;
    }

    bool Texture::hasFilters(uint16_t f) const {
        return (filters & f);
    }

    bool Texture::hasChannels(uint8_t c) const {
        return (channels & c);
    }

    long double Texture::getContrast() const {
        return contrast;
    }

    long double Texture::getSaturation() const {
        return saturation;
    }

    long double Texture::getBrightness() const {
        return brightness;
    }

    Texture &Texture::setContrast(long double v) {
        contrast = v;
        return *this;
    }

    Texture &Texture::setSaturation(long double v) {
        saturation = v;
        return *this;
    }

    Texture &Texture::setBrightness(long double v) {
        brightness = v;
        return *this;
    }

    Texture::~Texture() {}

    void Texture::notifyLayers() {

    }
}
