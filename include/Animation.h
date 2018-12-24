#ifndef JGL_ANIMATION_H
#define JGL_ANIMATION_H

#include <Core/jutil.h>

namespace jgl {
    class Animation {
    public:

        enum PlayMode : uint16_t {
            FORWARD     = 0x01,
            BACKWARD    = 0x02,
            PAUSED      = 0x04,
            //OR-ing 0x08 causes the animation to restart once finished in either direction.
            FORWARD_REPEAT  = 0x01 | 0x08,
            BACKWARD_REPEAT = 0x02 | 0x08,
            //OR-ing 0x10 causes the animation to change directions once finished.
            FORWARD_ALTERNATE   = 0x01 | 0x10,
            BACKWARD_ALTERNATE  = 0x02 | 0x10
        };

        Animation(long double, long double = 1.L, PlayMode = FORWARD);
        Animation(long double, PlayMode);

        virtual void animate(long double) = 0;

        void play();
        void pause();
        void stop();

        const PlayMode &getPlayMode() const;
        void setPlayMode(PlayMode);

        const size_t &getFrameCounter() const;

        static jutil::Queue<Animation*> &getPlayingAnimations();

        const long double &getLength() const;
        const long double &getSpeedMultiplier() const;
        void setSpeedMultiplier(long double);

        long double getProgress();

        long double getTimeElapsed();

        bool playing() const;

        void step();
    protected:
        long double length, multiplier;
    private:
        size_t framesCounted;
        long double playTime, TimeAtLastFrame;
        PlayMode playMode;
        jutil::Timer timer;

        void advanceFrameCounter();
        void reverseFrameCounter();

        void progress();
        void regress();
    };
}

#endif // JGL_ANIMATION_H
