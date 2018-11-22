#ifndef JGL_TEXTURE_LAYER_H
#define JGL_TEXTURE_LAYER_H

#include "Texture.h"

namespace jgl {

    class CompositeTexture;

    enum DefaultSize {
        MANUAL,
        RELATIVE,
        ABSOLUTE
    };

    enum Slot : uint32_t {

        //The number of available slots will vary depending on the system.
        //All systems that support use of JGL will support at least 12 texture slots (3 maps).
        //Use jgl::supportedTextureUnits() to see how many are enabled at run-time.
        //The number of supported map slots will always be 1/4 the supported texture slots.

        // ==  NOTE: SLOTS ARE NUMBERED IN HEXADECIMAL ==

        //Minimum slots (segments 0 - 2)
        //----------------- Map_0
        SLOT_0,     //RED
        SLOT_1,     //GREEN
        SLOT_2,     //BLUE
        SLOT_3,     //BLACK
        //----------------- Map_1
        SLOT_4,     //RED
        SLOT_5,     //GREEN
        SLOT_6,     //BLUE
        SLOT_7,     //BLACK
        //----------------- Map_2
        SLOT_8,     //RED
        SLOT_9,     //GREEN
        SLOT_A,     //BLUE
        SLOT_B,     //BLACK

        //Standard slots (segments 3 - 5)
        //----------------- Map_3
        SLOT_C,     //RED
        SLOT_D,     //GREEN
        SLOT_E,     //BLUE
        SLOT_F,     //BLACK
        //----------------- Map_4
        SLOT_10,     //RED
        SLOT_11,     //GREEN
        SLOT_12,     //BLUE
        SLOT_13,     //BLACK
        //----------------- Map_5
        SLOT_14,     //RED
        SLOT_15,     //GREEN
        SLOT_16,     //BLUE
        SLOT_17,     //BLACK

        //Extended slots (segments 6 - 8)
        //----------------- Map_6
        SLOT_18,     //RED
        SLOT_19,     //GREEN
        SLOT_1A,     //BLUE
        SLOT_1B,     //BLACK
        //----------------- Map_7
        SLOT_1C,     //RED
        SLOT_1D,     //GREEN
        SLOT_1E,     //BLUE
        SLOT_1F,     //BLACK
        //----------------- Map_8
        SLOT_20,     //RED
        SLOT_21,     //GREEN
        SLOT_22,     //BLUE
        SLOT_23,     //BLACK

        //Maximum slots (segments 9 - B)
        //----------------- Map_9
        SLOT_24,     //RED
        SLOT_25,     //GREEN
        SLOT_26,     //BLUE
        SLOT_27,     //BLACK
        //----------------- Map_A
        SLOT_28,     //RED
        SLOT_29,     //GREEN
        SLOT_2A,     //BLUE
        SLOT_2B,     //BLACK
        //----------------- Map_B
        SLOT_2C,     //RED
        SLOT_2D,     //GREEN
        SLOT_2E,     //BLUE
        SLOT_2F,     //BLACK

        //Map Slots
        MAP_0 = SLOT_0,
        MAP_1,
        MAP_2,
        MAP_3,
        MAP_4,
        MAP_5,
        MAP_6,
        MAP_7,
        MAP_8,
        MAP_9,
        MAP_A,
        MAP_B,

        //Segment identifiers
        SEGMENT_0 = SLOT_0,
        SEGMENT_1,
        SEGMENT_2,
        SEGMENT_3,
        SEGMENT_4,
        SEGMENT_5,
        SEGMENT_6,
        SEGMENT_7,
        SEGMENT_8,
        SEGMENT_9,
        SEGMENT_A,
        SEGMENT_B
    };

    enum MapChannel : uint32_t {
        MAP_RED_0,
        MAP_GREEN_0,
        MAP_BLUE_0,
        MAP_BLACK_0,
        MAP_RED_1,
        MAP_GREEN_1,
        MAP_BLUE_1,
        MAP_BLACK_1,
        MAP_RED_2,
        MAP_GREEN_2,
        MAP_BLUE_2,
        MAP_BLACK_2,
        MAP_RED_3,
        MAP_GREEN_3,
        MAP_BLUE_3,
        MAP_BLACK_3,
        MAP_RED_4,
        MAP_GREEN_4,
        MAP_BLUE_4,
        MAP_BLACK_4,
        MAP_RED_5,
        MAP_GREEN_5,
        MAP_BLUE_5,
        MAP_BLACK_5,
        MAP_RED_6,
        MAP_GREEN_6,
        MAP_BLUE_6,
        MAP_BLACK_6,
        MAP_RED_7,
        MAP_GREEN_7,
        MAP_BLUE_7,
        MAP_BLACK_7,
        MAP_RED_8,
        MAP_GREEN_8,
        MAP_BLUE_8,
        MAP_BLACK_8,
        MAP_RED_9,
        MAP_GREEN_9,
        MAP_BLUE_9,
        MAP_BLACK_9,
        MAP_RED_A,
        MAP_GREEN_A,
        MAP_BLUE_A,
        MAP_BLACK_A,
        MAP_RED_B,
        MAP_GREEN_B,
        MAP_BLUE_B,
        MAP_BLACK_B
    };

    enum _MapChannelInternal : uint32_t {
        _MAP_RED,
        _MAP_GREEN,
        _MAP_BLUE,
        _MAP_ALPHA,
        _MAP_BLACK
    };

    enum TextureLayerUpdate : uint16_t {
        NO_UPDATES      = 0x00,
        SCALING_FACTOR  = 0x01,
        SIZE            = 0x02,
        ROTATION        = 0x04,
        POSITION        = 0x08,
        SIZE_MODE       = 0x10,
        SLOT_ID         = 0x20,
        TEXTURE_PTR     = 0x40,
        COMPOSITE_PTR   = 0x80,
        ALL_DATA        = 0xff
    };

    struct TextureLayer : public Transformable {
        TextureLayer(size_t);
        TextureLayer(Texture*, CompositeTexture*, size_t);

        void recreate(Texture*);

        ~TextureLayer();

        DefaultSize getScalingMode() const;
        const Dimensions &getImageSize() const;
        TextureLayer &setScalingMode(DefaultSize);
        Scalable &setSize(const Dimensions&) override;
        Scalable &scale(const jml::Vector2f&) override;
        const jml::Vector2f &getScalingFactor() const;
        TextureLayer &setScalingFactor(const jml::Vector2f&);

        Translatable &setPosition(const Position&) override;
        Rotatable &setRotation(const jml::Angle&) override;

        MapChannel getMapChannel() const;
        Slot getSlot() const;
        TextureLayer &reseat(uint32_t);

        TextureLayer &operator=(const TextureLayer&);
        Texture *const getTexture() const;
        CompositeTexture *const getCompositer() const;
        TextureLayer &setTexture(Texture*);
        TextureLayer &setCompositer(CompositeTexture*);

    protected:
        virtual void onUpdate(uint16_t);
        jml::Vector2f scalingFactor;
        DefaultSize sMode;
        uint32_t slot;
        Texture *texture;
        CompositeTexture *container;
    };
}

#endif // JGL_TEXTURE_LAYER_H
