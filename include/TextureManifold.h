#ifndef JGL_TEXTURE_MANIFOLD_H
#define JGL_TEXTURE_MANIFOLD_H

#include "TextureLayer.h"

#define JGL_DEFAULT_TEXTURE_INDEX -128

namespace jgl {
    class TextureManifold {
    public:
        TextureManifold();

        TextureManifold &addTexture(Texture*, int8_t = JGL_DEFAULT_TEXTURE_INDEX);
        TextureManifold &removeTexture(Texture*);
        bool hasTexture(const Texture*) const;


        /// Transformation methods
        TextureManifold &setTexturePosition(const Texture*, const Position&);
        TextureManifold &moveTexture(const Texture*, const Position&);

        TextureManifold &setTextureSize(const Texture*, const Dimensions&);
        TextureManifold &scaleTexture(const Texture*, const jml::Vector2f&);

        TextureManifold &setTextureRotation(const Texture*, const jml::Angle&);
        TextureManifold &rotateTexture(const Texture*, const jml::Angle&);

        Position getTexturePosition(const Texture*) const;
        Dimensions getTextureSize(const Texture*) const;
        jml::Angle getTextureRotation(const Texture*) const;


        size_t numLayers() const;

        virtual ~TextureManifold();
    protected:
        jutil::Queue<TextureLayer> layers;
        virtual void updateLayers();
    };
}

#endif // JGL_TEXTURE_MANIFOLD_H
