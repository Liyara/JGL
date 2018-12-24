#include "Animation.h"
#include <Maths.h>

#define PLAYMODE_OR(a, b)\
    static_cast<uint16_t>(a) | static_cast<uint16_t>(b)

#define PLAYMODE_OR_EQUAL(a, b)\
    a = static_cast<PlayMode>(PLAYMODE_OR(a, b))

#define PLAYMODE_AND(a, b)\
    static_cast<uint16_t>(a) & static_cast<uint16_t>(b)

#define PLAYMODE_AND_EQUAL(a, b)\
    a = static_cast<PlayMode>(PLAYMODE_AND(a, b))

#define PLAYMODE_NOT(a)\
    ~static_cast<uint16_t>(a)

namespace jgl {

    jutil::Queue<Animation*> _playing;

    Animation::Animation(long double l, long double m, Animation::PlayMode p) : length(l), multiplier(m), playMode(p), framesCounted(0), TimeAtLastFrame(0) {}

    void Animation::play() {
        PLAYMODE_AND_EQUAL(playMode, PLAYMODE_NOT(PAUSED));
        timer.start();
        timer.set(TimeAtLastFrame, jutil::MILLISECONDS);
        if (_playing.empty() || !_playing.find(this)) _playing.insert(this);
    }

    void Animation::stop() {
        for (auto it = _playing.begin(); it != _playing.end(); ++it) {
            if (*it == this) {
                _playing.erase(it);
                break;
            }
        }
        playTime = 0;
        framesCounted = 0;
        TimeAtLastFrame = 0;
        timer.stop();
    }

    void Animation::pause() {
        TimeAtLastFrame = timer.stop(jutil::MILLISECONDS);
        PLAYMODE_OR_EQUAL(playMode, PAUSED);
    }

    const Animation::PlayMode &Animation::getPlayMode() const {
        return playMode;
    }

    jutil::Queue<Animation*> &Animation::getPlayingAnimations() {
        return _playing;
    }

    const size_t &Animation::getFrameCounter() const {
        return framesCounted;
    }

    void Animation::advanceFrameCounter() {
        ++framesCounted;
    }
    void Animation::reverseFrameCounter() {
        if (framesCounted > 0) --framesCounted;
    }

    long double Animation::getTimeElapsed() {
        if (!playing()) return 0;
        else return playTime;
    }

    long double Animation::getProgress() {
       return getTimeElapsed() / length;
    }

    void Animation::progress() {
        if (!(playMode & PAUSED)) {
            advanceFrameCounter();
            long double
                gt = timer.get(jutil::MILLISECONDS) * multiplier,
                passed = jml::abs(gt - TimeAtLastFrame);
            ;

            playTime += passed;

            auto cmp = jml::compare(getProgress(), 1.L);
            if (cmp == JML_GREATER) {
                if (playMode & 0x08) {
                    playTime = 0;
                    TimeAtLastFrame = 0;
                    timer.stop();
                    timer.start();
                } else if ((playMode & 0x10)) {
                    playTime = length;
                    TimeAtLastFrame = 0;
                    PLAYMODE_AND_EQUAL(playMode, PLAYMODE_NOT(FORWARD));
                    PLAYMODE_OR_EQUAL(playMode, BACKWARD);
                } else stop();
            } else animate(getProgress());

            TimeAtLastFrame = gt;
        }
    }

    void Animation::step() {
        if (playMode & FORWARD) {
            progress();
        } else if (playMode & BACKWARD) {
            regress();
        }
    }

    void Animation::regress() {
        if (!(playMode & PAUSED)) {
            reverseFrameCounter();
            long double
                gt = timer.get(jutil::MILLISECONDS) * multiplier,
                passed = jml::abs(gt - TimeAtLastFrame);
            ;

            playTime -= passed;

            auto cmp = jml::compare(getProgress(), 0.L);
            if (cmp == JML_EQUAL || cmp == JML_LESS) {
                if (playMode & 0x08) {
                    playTime = length;
                    TimeAtLastFrame = 0;
                    timer.stop();
                    timer.start();
                } else if ((playMode & 0x10)) {
                    playTime = 0;
                    TimeAtLastFrame = 0;
                    PLAYMODE_AND_EQUAL(playMode, PLAYMODE_NOT(BACKWARD));
                    PLAYMODE_OR_EQUAL(playMode, FORWARD);
                }
            } else animate(getProgress());

            TimeAtLastFrame = gt;
        }
    }

    bool Animation::playing() const {
        return !(_playing.empty() || !_playing.find(const_cast<Animation*>(this)));
    }

    void Animation::setPlayMode(Animation::PlayMode p) {
        playMode = p;

    }

    const long double &Animation::getLength() const {
        return length;
    }

    const long double &Animation::getSpeedMultiplier() const {
        return multiplier;
    }

    void Animation::setSpeedMultiplier(long double m) {
        multiplier = m;
    }

}
