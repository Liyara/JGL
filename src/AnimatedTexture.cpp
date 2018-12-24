#include "AnimatedTexture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


namespace jgl {

    AnimatedTextureFrame::AnimatedTextureFrame(const Image &img, long double f) : Image(img), duration(f) {

    }

    AnimatedTextureFrame::AnimatedTextureFrame(Image &&img, long double f) : Image(jutil::move(img)), duration(f) {

    }

    AnimatedTextureFrame::AnimatedTextureFrame(long double f) : duration(f) {}

    AnimatedTexture::AnimatedTexture() : Animation(0), currentFrame(nullptr), currentFrameCounter(0) {

    }

    AnimatedTexture::AnimatedTexture(const jutil::String &s) : Animation(0), currentFrameCounter(0) {
        load(s);
        currentFrame = frames[0];
        length = calcTotalDuration();
    }

    void AnimatedTexture::animate(long double percent) {
        //jutil::out << percent << jutil::endl;
        long double realTime = percent * getLength();
        AnimatedTextureFrame *mappedFrame = getFrameAtTime(realTime);
        if (mappedFrame) currentFrame = mappedFrame;
    }

    AnimatedTextureFrame *AnimatedTexture::getFrameAtTime(long double time) {
        while (time > getLength()) time -= getLength();
        long double passed = 0;
        char cmp;
        for (auto &i: frames) {
            passed += i->duration;
            cmp = jml::compare(passed, time);
            if (cmp == JML_GREATER || cmp == JML_EQUAL) return i;
        }
        return nullptr;
    }

    bool AnimatedTexture::setDelay(size_t i, long double d) {
        if (i == JGL_LAST_FRAME) {
            if (!frames.empty()) frames.last()->duration = d;
            else return false;
        } else if (i == JGL_FIRST_FRAME) {
            if (!frames.empty()) frames.first()->duration = d;
            else return false;
        } else if (i == JGL_CURRENT_FRAME) {
            if (currentFrame) currentFrame->duration = d;
            else return false;
        } else if (i == JGL_ALL_FRAMES) {
            for (auto &i: frames) i->duration = d;
        } else {
            if (i < frames.size()) frames[i]->duration = d;
            else return false;
        }
        return true;
    }

    const Image::Handle &AnimatedTexture::handle() {
        return currentFrame->handle();
    }

    const ResourceID &AnimatedTexture::id() const {
        return currentFrame->id();
    }

    bool AnimatedTexture::addFrame(const Image &img, long double len, size_t i) {
        if (i == JGL_LAST_FRAME || i <= frames.size()) {
            if (i == JGL_LAST_FRAME) {
                frames.insert(new AnimatedTextureFrame(img, len));
            } else {
                frames.insert(new AnimatedTextureFrame(img, len), i);
            }
            length = calcTotalDuration();
            if (!currentFrame) currentFrame = frames[0];
            return true;
        } else return false;
    }

    bool AnimatedTexture::addFrame(const Texture &t, long double len, size_t i) {
        return addFrame(t.getUnderlyingImage(), len, i);
    }

    bool AnimatedTexture::addFrame(Image &&img, long double len, size_t i) {
        if (i == JGL_LAST_FRAME || i <= frames.size()) {
            if (i == JGL_LAST_FRAME) {
                frames.insert(new AnimatedTextureFrame(jutil::move(img), len));
            } else {
                frames.insert(new AnimatedTextureFrame(jutil::move(img), len), i);
            }
            length = calcTotalDuration();
            if (!currentFrame) currentFrame = frames[0];
            return true;
        } else return false;
    }

    bool AnimatedTexture::removeFrame(size_t i) {
        if (i < frames.size()) {
            frames.erase(i);
            length = calcTotalDuration();
            if (frames.empty() && currentFrame) currentFrame = nullptr;
            return true;
        } else return false;
    }

    long double AnimatedTexture::calcTotalDuration() {
        long double r = 0;
        for (auto &i: frames) {
            r += i->duration;
        }
        return r;
    }

    AnimatedTexture::~AnimatedTexture() {
        for (auto &i: frames) {
            delete i;
            i = nullptr;
        }
    }

    Texture &AnimatedTexture::load(const jutil::String &filename) {
        frames.clear();
        FILE *file = NULL;
        char cFilename[filename.size() + 1];
        filename.array(cFilename);
        stbi__context s;
        file = stbi__fopen(cFilename, "rb");

        jutil::out << jutil::String(cFilename) << jutil::endl;

        stbi__start_file(&s, file);

        if (stbi__gif_test(&s)) {
            int c;
            stbi__gif rawGif;

            memset(&rawGif, 0, sizeof(rawGif));

            uint8_t *data, *frameData;

            for (size_t i = 0; (data = stbi__gif_load_next(&s, &rawGif, &c, 4, NULL)); ++i) {
                if (data == (unsigned char*)&s) break;

                size_t bDataSize = rawGif.w * rawGif.h * 4;

                frameData = (uint8_t*) malloc(bDataSize);

                memcpy(frameData, data, bDataSize);

                ByteArray bData(frameData, bDataSize);

                frames.insert(new AnimatedTextureFrame(Image(bData, {static_cast<unsigned>(rawGif.w), static_cast<unsigned>(rawGif.h)}), static_cast<long double>(rawGif.delay)));
            }

            STBI_FREE(rawGif.out);

        } else {
            if (currentFrame) {
                Texture::load(filename);
                ColorArray dat = rawData.getImageData();
                currentFrame->setImageData(dat);
                currentFrame->setSize(rawData.getSize());
            }
        }

        length = calcTotalDuration();

        fclose(file);

        return *this;

    }

    Texture &AnimatedTexture::load(const Image &img) {
        if (currentFrame) {
            Texture::load(img);
            ColorArray dat = rawData.getImageData();
            currentFrame->setImageData(dat);
            currentFrame->setSize(rawData.getSize());
        }
        return *this;
    }

    Texture &AnimatedTexture::load(const Texture &tex) {
        if (currentFrame) {
            Texture::load(tex);
            ColorArray dat = rawData.getImageData();
            currentFrame->setImageData(dat);
            currentFrame->setSize(rawData.getSize());
        }
        return *this;
    }

    Texture &AnimatedTexture::load(Texture &&tex)  {
        if (currentFrame) {
            Texture::load(jutil::move(tex));
            ColorArray dat = rawData.getImageData();
            currentFrame->setImageData(dat);
            currentFrame->setSize(rawData.getSize());
        }
        return *this;
    }

    Texture &AnimatedTexture::load(Image &&img) {
        if (currentFrame) {
            Texture::load(jutil::move(img));
            ColorArray dat = rawData.getImageData();
            currentFrame->setImageData(dat);
            currentFrame->setSize(rawData.getSize());
        }
        return *this;
    }

    Texture &AnimatedTexture::load(const Shader &s) {
        if (currentFrame) {
            Texture::load(s);
            ColorArray dat = rawData.getImageData();
            currentFrame->setImageData(dat);
            currentFrame->setSize(rawData.getSize());
        }
        return *this;
    }

    Texture &AnimatedTexture::load(const Screen &s) {
        if (currentFrame) {
            Texture::load(s);
            ColorArray dat = rawData.getImageData();
            currentFrame->setImageData(dat);
            currentFrame->setSize(rawData.getSize());
        }
        return *this;
    }

    const Image &AnimatedTexture::getUnderlyingImage() const {
        return *currentFrame;
    }

    Image &AnimatedTexture::operator*() {
        return *currentFrame;
    }

    Image *AnimatedTexture::operator->() {
        return currentFrame;
    }


}








