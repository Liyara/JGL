#include "TextureLayer.h"
#include "Dependencies.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FILL_TEXTURE(_dat_) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &((static_cast<jutil::Queue<uint32_t> >(_dat_))[0]))
#define FILL_TEXTURE_NULL glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);

namespace jgl {

    Texture::Texture() : Scalable(0), rawData() {

    }
    Texture::Texture(const Image &img) : Scalable(img.getSize()), rawData(img) {

    }
    Texture::Texture(const jutil::String &file) : Scalable(0) {
        load(file);
    }
    Texture::Texture(const Texture &t) : Scalable(t.size), rawData(t.rawData) {

    }
    Texture::Texture(const Shader&) : Scalable(0), rawData() {
        // Not yet complete
    }
    Texture::Texture(const Screen&) : Scalable(0), rawData() {
        // Not yet complete
    }
    Texture::Texture(Texture &&t) : Scalable(t.size), rawData(jutil::move(t.rawData)) {
        t.size = 0;
    }

    Texture &Texture::operator=(const Texture &t) {
        size = t.size;
        rawData = t.rawData;
        return *this;
    }
    Texture &Texture::operator=(Texture &&t) {
        size = t.size;
        rawData = jutil::move(t.rawData);
        t.size = 0;
        return *this;
    }

    //Retrieve the underlying Image of the current frame
    Image &Texture::operator*() {
        return rawData;
    }
    Image *Texture::operator->() {
        return &rawData;
    }

    //Change the image data of the specified texture frame
    Texture &Texture::load(const jutil::String &s) {
        char ss[s.size() + 1];
        s.array(ss);
        int nrChannels;
        jml::Vector2u ssize;
        uint8_t *sData = stbi_load(ss, reinterpret_cast<int*>(&(ssize[0])), reinterpret_cast<int*>(&(ssize[1])), &nrChannels, STBI_default);

        rawData.setImageData(ByteArray(sData, (ssize[0] * ssize[1]) * nrChannels), ssize, nrChannels);
        size = rawData.getSize();

        if (rawData._handle) {
            glMakeTextureHandleNonResidentARB(rawData._handle);
            glGetTextureSamplerHandleARB(rawData._id, rawData._handle);
            glMakeTextureHandleResidentARB(rawData._handle);
        }

        return *this;
    }
    Texture &Texture::load(const Image &img) {
        rawData.setImageData(static_cast<WordArray>(img.getImageData()), img.getSize());
        size = rawData.getSize();
        return *this;
    }
    Texture &Texture::load(const Texture &t) {
        return load(t.rawData);
    }
    Texture &Texture::load(Texture &&t) {
        rawData = jutil::move(t.rawData);
        size = rawData.getSize();
        return *this;
    }

    Texture &Texture::load(const Shader&) {
        // Not finished
    }
    Texture &Texture::load(const Screen&) {
        // Not finished
    }

    const ResourceID &Texture::id() const {
        return rawData.id();
    }

    const Image::Handle &Texture::handle() {
        return rawData.handle();
    }

    Texture::~Texture() {}

    size_t supportedTextureUnits() {
        int i;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i);
        return static_cast<size_t>(i);
    }
}
