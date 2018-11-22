#include "CompositeTexture.h"
#include "Maths.h"
#include <IO/IO.h>

using namespace jml::literals;

namespace jgl {

    Image *dmi;
    Texture *dmt;

    TextureLayer CompositeTexture::JGL_NULL_LAYER(0x40);
    BlendingMode CompositeTexture::JGL_NULL_BLENDER(static_cast<BlendingMode>(-1));

    void initTextures() {
        dmi = new Image(WordArray{0, 0, 0, 0}, 2);
        dmt = new Texture(*dmi);
    }

    void destroyTextures() {
        delete dmt;
        delete dmi;
    }

    TextureLayerLink::TextureLayerLink(uint32_t index, TextureLayerLink *p, TextureLayerLink *n) : layers(TextureLayer(index), TextureLayer(index + 1), TextureLayer(index + 2), TextureLayer(index + 3)), previous(p), next(n), map(index / 4) {
        map.setTexture(dmt);
    }

    bool TextureLayerLink::insertTexture(Texture *tex, uint32_t i) {
        layers[i].setTexture(tex);
        return true;
    }

    bool TextureLayerLink::removeTexture(Texture*, uint32_t i) {
        layers[i].setTexture(nullptr);
        return true;
    }

    TextureLayerLink::~TextureLayerLink() {

    }

    bool CompositeTexture::validSlot(uint32_t i) {
        size_t units = supportedTextureUnits();
        return i < (units - (units / 4));
    }

    uint32_t CompositeTexture::slotToSegment(uint32_t slot) {
        return (slot / 4);
    }

    uint32_t CompositeTexture::slotToLayer(uint32_t slot) {
        return (slot % 4);
    }

    CompositeTexture::CompositeTexture() : representative(this) {
        size_t units = supportedTextureUnits();
        units -= units / 4;
        TextureLayerLink *cur = nullptr;
        for (size_t i = 0; i < units; i += 4) {
            segments.insert(TextureLayerLink(i, cur, nullptr));
            if (cur) cur->next = &(segments.last());
            cur = &(segments.last());
        }
    }

    bool CompositeTexture::addTexture(Texture *tex, uint32_t i) {
        if (i == JGL_DEFAULT_TEXTURE_INDEX) {
            for (auto &i: segments) {
                for (auto &ii: i.layers) {
                    if(!ii.getTexture()) {
                        ii.setTexture(tex);
                        return true;
                    }
                }
            }
            return false;
        } else {
            if (validSlot(i)) {
                (*this)[i].setTexture(tex);
                return true;
            } else return false;
        }
    }

    bool CompositeTexture::addTextures(jutil::Queue<Texture*> texs, uint32_t slot) {
        for (size_t i = 0; i < texs.size(); ++i) {
            if (!addTexture(texs[i], (slot == JGL_DEFAULT_TEXTURE_INDEX? slot : slot + i))) return false;
        }
        return true;
    }

    bool CompositeTexture::removeTexture(Texture *tex) {
        for (auto &i: segments) {
            for (auto &ii: i.layers) {
                if (ii.getTexture() == tex) {
                    ii.setTexture(nullptr);
                    return true;
                }
            }
        }
        return false;
    }

    bool CompositeTexture::swap(uint32_t tex1Slot, uint32_t tex2Slot) {
        if (validSlot(tex1Slot) && validSlot(tex2Slot)) {
            TextureLayer layer = (*this)[tex1Slot];
            (*this)[tex1Slot] = (*this)[tex2Slot];
            (*this)[tex2Slot] = layer;
            return true;
        } else return false;
    }

    bool CompositeTexture::clear(uint32_t i) {
        if (validSlot(i)) {
            (*this)[i].setTexture(nullptr);
            return true;
        } else return false;
    }

    bool CompositeTexture::clear() {
        for (size_t i = 0; validSlot(i); ++i) clear(i);
        return true;
    }

    bool CompositeTexture::setMap(Texture *map, size_t slot) {
        if (slot == JGL_DEFAULT_TEXTURE_INDEX) {
            for (size_t i = 0; i < segments.size(); ++i) {
                if (!segments[i].map.getTexture()) {
                    segments[i].map.setTexture(map);
                    return true;
                }
            }
            return false;
        } else {
            if (slot <= MAP_B) {
                segments[slot].map.setTexture(map);
                return true;
            } else return false;
        }
    }

    TextureLayer &CompositeTexture::operator[](uint32_t i) {
        return segments[slotToSegment(i)].layers[slotToLayer(i)];
    }

    bool CompositeTexture::validSegment(uint32_t segment) {
        size_t units = supportedTextureUnits();
        return segment < (units / 4);
    }

    bool CompositeTexture::setBlender(BlendingMode blender, uint32_t segment, uint32_t phase) {
        if (segment < segments.size()) {
            if (phase == PHASE_0 || phase == ALL_PHASES) {
                segments[segment].linkBlender = blender;
            }
            if (phase == PHASE_1 || phase == ALL_PHASES) {
                segments[segment].connectBlender = blender;
            }
            return true;
        } else return false;
    }

    const BlendingMode &CompositeTexture::getBlender(uint32_t segment, uint32_t phase) {
        switch(static_cast<BlendPhase>(phase)) {
        case PHASE_0:;
            if (segment < segments.size()) {
                return segments[segment].linkBlender;
            } else return JGL_NULL_BLENDER;

        case PHASE_1:;
            if (segment < segments.size() - 1) {
                return segments[segment].connectBlender;
            } else return JGL_NULL_BLENDER;

        default: return JGL_NULL_BLENDER;
        }
    }

    void RepresentationLayer::onUpdate(uint16_t updates) {
        container->performUpdates(updates, *this);
    }

    CompositeTexture &CompositeTexture::performUpdates(uint16_t updates, const TextureLayer &data) {
        for (auto &i: segments) {
            for (auto &ii: i.layers) {
                if (ii.getTexture()) {
                    if (updates & SIZE) ii.setSize(data.getSize());
                    if (updates & SCALING_FACTOR) ii.setScalingFactor(data.getScalingFactor());
                    if (updates & ROTATION) ii.setRotation(data.getRotation());
                    if (updates & POSITION) ii.setPosition(data.getPosition());
                    if (updates & SIZE_MODE) ii.setScalingMode(data.getScalingMode());
                    if (updates & SLOT_ID) ii.reseat(static_cast<uint32_t>(data.getSlot()));
                    if (updates & TEXTURE_PTR) ii.setTexture(data.getTexture());
                    if (updates & COMPOSITE_PTR) ii.setCompositer(data.getCompositer());
                }
            }
        }
        return *this;
    }

    uint32_t CompositeTexture::availableSlots() {
        size_t units = supportedTextureUnits();
        return units - (units / 4);
    }

    bool CompositeTexture::swap(Texture *tex1, Texture *tex2) {
        if (tex1 == tex2) return true;
        uint32_t tex1Slot = JGL_DEFAULT_TEXTURE_INDEX, tex2Slot = JGL_DEFAULT_TEXTURE_INDEX;
        for (auto &i: segments) {
            for (auto &ii: i.layers) {
                if (ii.getTexture() == tex1) tex1Slot = ii.getSlot();
                else if (ii.getTexture() == tex2) tex2Slot = ii.getSlot();
            }
        }
        if (tex1Slot == JGL_DEFAULT_TEXTURE_INDEX || tex2Slot == JGL_DEFAULT_TEXTURE_INDEX) return false;
        else return swap(tex1Slot, tex2Slot);
    }

    bool CompositeTexture::swap(Texture *tex1, uint32_t tex2Slot) {
        uint32_t tex1Slot = JGL_DEFAULT_TEXTURE_INDEX;
        for (auto &i: segments) for (auto &ii: i.layers) if (ii.getTexture() == tex1) tex1Slot = ii.getSlot();
        if (tex1Slot == JGL_DEFAULT_TEXTURE_INDEX) return false;
        else return swap(tex1Slot, tex2Slot);
    }

    TextureLayer &CompositeTexture::operator[](Texture *tex) {
        if (tex) {
            for (auto &i: segments) for (auto &ii: i.layers) if (ii.getTexture() == tex) return ii;
        } else {
            return representative;
        }
        return JGL_NULL_LAYER;
    }

    bool CompositeTexture::setBlender(BlendingMode b) {
        for (auto &i: segments) {
            i.linkBlender = b;
            i.connectBlender = b;
        }
        return true;
    }

    RepresentationLayer::RepresentationLayer(CompositeTexture *c) : TextureLayer(nullptr, c, JGL_DEFAULT_TEXTURE_INDEX) {}
}
