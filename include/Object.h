#ifndef OBJECT_H
#define OBJECT_H

#include "Shader.h"

typedef jml::Vector2u Dimensions;
typedef jml::Vector2ld Position;

#include "color.hpp"
#include "Material.h"

namespace jgl {

    class Object {
    public:

        Object(const Position&, const Dimensions&, const Color&);
        virtual Object &setColor(const Color&);
        virtual Color getColor() const;
        virtual Object &rotate(const jml::Angle&);
        virtual jml::Angle getRotation() const;
        virtual Object &setRotation(const jml::Angle&);
        virtual Object &setPosition(const Position&);
        virtual Object &setSize(const Dimensions&);
        virtual Dimensions getSize() const;
        virtual Position getPosition() const;
        virtual Object &move(Position);
        virtual Object &scale(jml::Vector2d);
        virtual Object &setTexture(const char*);
        virtual Object &setTexture(GLuint);
        virtual GLuint *getTexture() const;
        virtual Object &setMaterial(const Material&);
        virtual jutil::Queue<jutil::Queue<long double> > getVAO() const;
        virtual long double area() const = 0;
        virtual Object &setFill(bool);
        virtual Object &setOutline(uint8_t);
        virtual uint8_t getOutline() const;
        virtual Object &setOutlineColor(const Color&);
        virtual Object &useShader(const Shader&);
        virtual Object &setMode(GLenum);
        virtual GLenum getMode() const;
        virtual ~Object();

        friend void jgl::display();

    protected:

        virtual jutil::Queue<jutil::Queue<long double> > genVAO() = 0;
        jutil::Queue<jutil::Queue<long double> > polygon;
        virtual Object &formShape();
        Shader shader;

        Position position;
        Dimensions size;
        Color color, outlineColor;

        bool hasTexture;

        Position initP;
        ILuint image;
        jml::Angle rotation;
        size_t vertexCount;
        virtual void draw();
        bool formed;
        virtual Object &generateMVP();
        jml::Matrix<float, 4, 4> mvp;
        unsigned components;
        jml::Vector<float, 3> norm;
        Material material;
        uint8_t outline;
        uint32_t texture, vao;
        bool fill;
        GLenum drawMode;
        Object();

    private:

    };
}

#endif // OBJECT_H
