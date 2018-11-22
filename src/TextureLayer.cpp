#include "TextureLayer.h"
#include "Maths.h"

using namespace jml::literals;

namespace jgl {

    TextureLayer::TextureLayer(size_t s) : Transformable(0, JGL_AUTOMATIC_SIZE, 0_degs), texture(nullptr), container(nullptr), scalingFactor(1), slot(s), sMode(MANUAL) {}

    TextureLayer::TextureLayer(Texture *t, CompositeTexture *c, size_t s) : Transformable(0, JGL_AUTOMATIC_SIZE, 0_degs), texture(t), container(c), scalingFactor(1), slot(s), sMode(MANUAL) {}

    TextureLayer &TextureLayer::operator=(const TextureLayer &l) {
        texture = l.texture;
        rotation = l.rotation;
        size = l.size;
        scalingFactor = l.scalingFactor;
        slot = l.slot;
        container = l.container;
        position = l.position;
        sMode = l.sMode;
        onUpdate(ALL_DATA);
    }

    void TextureLayer::recreate(Texture *t) {
        (*this) = TextureLayer(t, container, slot);
    }

    TextureLayer::~TextureLayer() {

    }

    DefaultSize TextureLayer::getScalingMode() const {
        return sMode;
    }

    const Dimensions &TextureLayer::getImageSize() const {
        return texture->getSize();
    }

    TextureLayer &TextureLayer::setScalingMode(DefaultSize m) {
        sMode = m;
        onUpdate(SIZE_MODE);
        return *this;
    }

    Scalable &TextureLayer::setSize(const Dimensions &d) {
        Scalable &r = Scalable::setSize(d);
        scalingFactor = {1.f, 1.f};
        onUpdate(SIZE | SCALING_FACTOR);
        return r;
    }

    Scalable &TextureLayer::scale(const jml::Vector2f &d) {
        scalingFactor[0] *= d[0];
        scalingFactor[1] *= d[1];
        onUpdate(SCALING_FACTOR);
        return *this;
    }

    const jml::Vector2f &TextureLayer::getScalingFactor() const {
        return scalingFactor;
    }

    MapChannel TextureLayer::getMapChannel() const {
        return static_cast<MapChannel>(slot);
    }

    Slot TextureLayer::getSlot() const {
        return static_cast<Slot>(slot);
    }

    void TextureLayer::onUpdate(uint16_t ups) {

    }

    TextureLayer &TextureLayer::reseat(uint32_t s) {
        slot = s;
        onUpdate(SLOT_ID);
        return *this;
    }

    Translatable &TextureLayer::setPosition(const Position &p) {
        Translatable::setPosition(p);
        onUpdate(POSITION);
        return *this;
    }
    Rotatable &TextureLayer::setRotation(const jml::Angle &a) {
        Rotatable::setRotation(a);
        onUpdate(ROTATION);
        return *this;
    }

    TextureLayer &TextureLayer::setScalingFactor(const jml::Vector2f &d) {
        scalingFactor[0] = d[0];
        scalingFactor[1] = d[1];
        onUpdate(SCALING_FACTOR);
        return *this;
    }

    Texture *const TextureLayer::getTexture() const {
        return texture;
    }

    CompositeTexture *const TextureLayer::getCompositer() const {
        return container;
    }
    TextureLayer &TextureLayer::setTexture(Texture *t) {
        texture = t;
        onUpdate(TEXTURE_PTR);
        return *this;
    }
    TextureLayer &TextureLayer::setCompositer(CompositeTexture *c) {
        container = c;
        onUpdate(COMPOSITE_PTR);
        return *this;
    }
}
