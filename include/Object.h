#ifndef OBJECT_H
#define OBJECT_H

#include "Window.h"
#include "Shader.h"
#include "Material.h"
#include "CompositeTexture.h"

#define _JGL_TEXTURE_SEGMENT            GL_TEXTURE0
#define _JGL_TEXTURE_SEGMENT_LENGTH     0x10

namespace jgl {

    class Object : public Renderable, public Transformable {
    public:

        Object(const Position&, const Dimensions&, const Color&);
        Object(const Object&);
        virtual Object &setColor(const Color&);
        virtual Color getColor() const;
        virtual Object &setMaterial(const Material&);
        virtual long double area() const = 0;
        virtual Object &setFill(bool);
        virtual Object &setOutline(uint8_t);
        virtual uint8_t getOutline() const;
        virtual Object &setOutlineColor(const Color&);
        virtual Object &useShader(Shader*);
        virtual jutil::Queue<jml::Vertex> getVertices();
        virtual Object &setOrigin(const jml::Vector2f&);
        virtual const jml::Vector2f &getOrigin() const;
        virtual Object &setTexture(Texture*);
        virtual Object &setTexture(CompositeTexture*);

        virtual ~Object();

        virtual void render() override;
        virtual void render(const Screen*);

        friend void jgl::display();

    protected:

        virtual jutil::Queue<jml::Vertex> generateVertices() const = 0;
        virtual Object &formShape();

        Texture *singleTexture;
        CompositeTexture *multiTexture;

        Material material;

        jutil::Queue<GLuint> uniforms;
        jutil::Queue<uint32_t> ubos;

        Shader *shader;

        jml::Vector<float, 3> norm;
        jml::Vector2f origin;

        Color color, outlineColor;

        uint8_t outline, tMode;
        uint32_t fbo;

        bool formed, fill, valid;

        Mesh mesh;

        struct FragmentDrawData;
        struct VertexDrawData;
        struct TextureDrawData;

        Object();

    private:

        jutil::Queue<jml::Vertex> textureObject;

    };
}

#endif // OBJECT_H
