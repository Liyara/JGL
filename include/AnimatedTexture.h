#ifndef JGL_ANIMATED_TEXTURE_H
#define JGL_ANIMATED_TEXTURE_H

#include "Animation.h"
#include "CompositeTexture.h"

namespace jgl {

    class AnimatedTextureFrame : public Image {
    public:
        AnimatedTextureFrame(long double);
        AnimatedTextureFrame(const Image&, long double);
        AnimatedTextureFrame(Image&&, long double);
        long double duration;
    };

    class AnimatedTexture : public Animation, public Texture {
    public:
        AnimatedTexture();
        AnimatedTexture(const jutil::String&);

        Image &operator*() override;
        Image *operator->() override;

        Texture &load(const jutil::String&) override;
        Texture &load(const Image&) override;
        Texture &load(const Texture&) override;
        Texture &load(Texture&&) override;
        Texture &load(Image&&) override;
        Texture &load(const Shader&) override;
        Texture &load(const Screen&) override;

        const Image &getUnderlyingImage() const override;

        bool addFrame(const Image&, long double, size_t = JGL_LAST_FRAME);
        bool addFrame(const Texture&, long double, size_t = JGL_LAST_FRAME);
        bool addFrame(Image&&, long double, size_t = JGL_LAST_FRAME);

        bool setDelay(size_t, long double);

        bool removeFrame(size_t);

        const Image::Handle &handle() override;
        const ResourceID &id() const override;

        virtual ~AnimatedTexture();

    private:
        void animate(long double) override;
        AnimatedTextureFrame *getFrameAtTime(long double);
        jutil::Queue<AnimatedTextureFrame*> frames;
        AnimatedTextureFrame *currentFrame;

        size_t currentFrameCounter;

        long double calcTotalDuration();
    };
}

#endif // JGL_ANIMATED_TEXTURE_H
