#ifndef JGL_TEXTURE_LAYER_H
#define JGL_TEXTURE_LAYER_H

#include "Texture.h"

#define JGL_AUTOMATIC_SIZE  (unsigned)(-1)

namespace jgl {

    class TextureManifold;

    enum TextureMode : uint8_t {
        MANUAL,         /// Texture coords specified by object.
        RELATIVE,       /// Generate texture coords to fit object vertices.
        ABSOLUTE        /// Keep texture size regardless of the objects's vertices.
    };

    class TextureLayer : public Transformable {
    public:
        TextureLayer(Texture*, const TextureManifold*);
        virtual ~TextureLayer();

        TextureLayer &setZIndex(int8_t);
        int8_t getZIndex() const;

        const TextureManifold *getManifold() const;

        const Texture *getTexture() const;

        const Dimensions &getImageSize() const;

        Scalable &scale(const jml::Vector2f &d) override;
        Scalable &setSize(const Dimensions &d) override;

        const jml::Vector2f &getScalingFactor() const;

        TextureLayer &setScalingMode(TextureMode);

        bool operator==(const TextureLayer&);
        bool operator>(const TextureLayer&);
        bool operator<(const TextureLayer&);

    private:

        Texture* texture;
        const TextureManifold *manifold;
        int8_t zIndex;
        TextureMode sMode;
        Dimensions tempSize;
        Position scalingFactor;

        friend class Texture;
    };
}

#endif // JGL_TEXTURE_LAYER_H
