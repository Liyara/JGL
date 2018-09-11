#include "TextureManifold.h"
#include "Maths.h"
#include <IO/IO.h>

#define TEXTURE_OPERATION(op) for (auto &i: layers) {if (i.getTexture() == t) {op;}}

using namespace jml::literals;

namespace jgl {
    TextureManifold::TextureManifold() {}

    TextureManifold &TextureManifold::addTexture(Texture *t, int8_t index) {
        if (index == JGL_DEFAULT_TEXTURE_INDEX) {
            if (layers.empty()) index = 0;
            else index = layers.last().getZIndex() + 1;
        }
        TextureLayer newLayer = TextureLayer(t, this);
        newLayer.setZIndex(index);
        if (layers.empty() || index > layers.last().getZIndex()) layers.insert(newLayer);
        else {
            for (size_t i = 0; i < layers.size(); ++i) {
                if (index <= layers[i].getZIndex()) {
                    layers.insert(newLayer, i);
                    break;
                }
            }
        }
        updateLayers();
        return *this;
    }
    TextureManifold &TextureManifold::removeTexture(Texture *t) {
        for (auto it = layers.begin(); it != layers.end(); ++it) {
            if (it->getTexture() == t) {
                layers.erase(it);
            }
        }
        updateLayers();
        return *this;
    }
    bool TextureManifold::hasTexture(const Texture *t) const {
        TEXTURE_OPERATION(return true);
        return false;
    }

    /// Transformation methods
    TextureManifold &TextureManifold::setTexturePosition(const Texture *t, const Position &p) {
        TEXTURE_OPERATION(i.setPosition(p));
        return *this;
    }

    TextureManifold &TextureManifold::moveTexture(const Texture *t, const Position &p) {
        TEXTURE_OPERATION(i.move(p));
        return *this;
    }

    TextureManifold &TextureManifold::setTextureSize(const Texture *t, const Dimensions &d) {
        TEXTURE_OPERATION(i.setSize(d));
        return *this;
    }

    TextureManifold &TextureManifold::scaleTexture(const Texture *t, const jml::Vector2f &d) {
        TEXTURE_OPERATION(i.scale(d));
        return *this;
    }

    TextureManifold &TextureManifold::setTextureRotation(const Texture *t, const jml::Angle &r) {
        TEXTURE_OPERATION(i.setRotation(r));
        return *this;
    }

    TextureManifold &TextureManifold::rotateTexture(const Texture *t, const jml::Angle &r) {
        TEXTURE_OPERATION(i.rotate(r));
        return *this;
    }

    Position TextureManifold::getTexturePosition(const Texture *t) const {
        TEXTURE_OPERATION(return i.getPosition());
        return -1;
    }

    Dimensions TextureManifold::getTextureSize(const Texture *t) const {
        TEXTURE_OPERATION(return i.getSize());
        return 0;
    }

    jml::Angle TextureManifold::getTextureRotation(const Texture *t) const {
        TEXTURE_OPERATION(return i.getRotation());
        return 0_rads;
    }

    size_t TextureManifold::numLayers() const {
        return layers.size();
    }

    void TextureManifold::updateLayers() {
        for (auto it = layers.begin(); it != layers.end(); ++it) if ((it + 1) != layers.end() && it->getZIndex() == (it + 1)->getZIndex()) (it + 1)->setZIndex((it + 1)->getZIndex() + 1);
    }

    TextureManifold::~TextureManifold() {}
}
