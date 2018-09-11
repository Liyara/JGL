#include "TextureLayer.h"
#include "Maths.h"

using namespace jml::literals;

namespace jgl {
    TextureLayer::TextureLayer(Texture *t, const TextureManifold *m) : Transformable(0, JGL_AUTOMATIC_SIZE, 0_degs), texture(t), manifold(m), scalingFactor(1) {
        t->containers.insert(this);
    }

    TextureLayer &TextureLayer::setZIndex(int8_t i) {
        zIndex = i;
        return *this;
    }

    int8_t TextureLayer::getZIndex() const {
        return zIndex;
    }

    const TextureManifold *TextureLayer::getManifold() const {
        return manifold;
    }

    const Texture *TextureLayer::getTexture() const {
        return texture;
    }

    TextureLayer::~TextureLayer() {
        for (auto it = texture->containers.begin(); it != texture->containers.end(); ++it) {
            if (*it == this) {
                texture->containers.erase(it);
                break;
            }
        }
    }

    const Dimensions &TextureLayer::getImageSize() const {
        return texture->size;
    }

    TextureLayer &TextureLayer::setScalingMode(TextureMode m) {
        sMode = m;
        return *this;
    }

    Scalable &TextureLayer::setSize(const Dimensions &d) {
        Scalable &r = Scalable::setSize(d);
        scalingFactor = {1.f, 1.f};
        return r;
    }

    Scalable &TextureLayer::scale(const jml::Vector2f &d) {
        scalingFactor[0] *= d[0];
        scalingFactor[1] *= d[1];
        return *this;
    }

    const jml::Vector2f &TextureLayer::getScalingFactor() const {
        return scalingFactor;
    }
}
