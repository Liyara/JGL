#ifndef JGL_COMPOSITE_TEXTURE_H
#define JGL_COMPOSITE_TEXTURE_H

#define JGL_DEFAULT_TEXTURE_INDEX 0x40

#include "TextureLayer.h"
#include <Container/Set.hpp>

namespace jgl {

    enum BlendingMode : uint32_t {
        STACKING,
        COMBINING,
        MAPPING
    };

    enum BlendPhase : uint32_t {
        PHASE_0,
        PHASE_1,
        ALL_PHASES
    };

    class TextureLayerLink {
    public:
        TextureLayerLink(uint32_t, TextureLayerLink*, TextureLayerLink*);
        virtual ~TextureLayerLink();
    private:
        bool insertTexture(Texture*, uint32_t);
        bool removeTexture(Texture*, uint32_t);
        jutil::Set<4, TextureLayer> layers;
        BlendingMode linkBlender, connectBlender;
        TextureLayer map;
        TextureLayerLink *next, *previous;
        friend class CompositeTexture;
    };

    class RepresentationLayer : public TextureLayer {
    public:
        RepresentationLayer(CompositeTexture*);
    private:
        void onUpdate(uint16_t) override;
    };

    class CompositeTexture {
    public:

        CompositeTexture();

        bool addTexture(Texture*, uint32_t = JGL_DEFAULT_TEXTURE_INDEX);
        bool addTextures(jutil::Queue<Texture*>, uint32_t = JGL_DEFAULT_TEXTURE_INDEX);
        bool removeTexture(Texture*);

        bool swap(Texture*, Texture*);

        bool swap(Texture*, uint32_t);

        bool swap(uint32_t, uint32_t);
        bool clear(uint32_t);
        bool clear();

        bool setMap(Texture*, size_t);

        TextureLayer &operator[](uint32_t);

        CompositeTexture &performUpdates(uint16_t, const TextureLayer&);

        TextureLayer &operator[](Texture*);

        bool setBlender(BlendingMode, uint32_t, uint32_t);
        bool setBlender(BlendingMode);
        const BlendingMode &getBlender(uint32_t, uint32_t);

        static uint32_t availableSlots();

        static bool validSlot(uint32_t);
        static bool validSegment(uint32_t);
        static uint32_t slotToSegment(uint32_t);
        static uint32_t slotToLayer(uint32_t);

    private:
        jutil::Queue<TextureLayerLink> segments;
        RepresentationLayer representative;
        static TextureLayer JGL_NULL_LAYER;
        static BlendingMode JGL_NULL_BLENDER;
    };

    void initTextures();
    void destroyTextures();
}


#endif // JGL_COMPOSITE_TEXTURE_H
