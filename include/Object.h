#ifndef OBJECT_H
#define OBJECT_H

#include "Dependencies.h"

typedef jml::Vector2u Dimensions;
typedef jml::Vector2f Position;

#include "color.hpp"
#include "Material.h"

namespace jgl {

    class Object {
    public:

        Object(Position, Dimensions, Color);
        Object &setColor(const Color&);
        virtual Color getColor() const;
        virtual Object &rotate(const jml::Angle&);
        virtual Object &setRotation(const jml::Angle&);
        virtual Object &setPosition(const Position&);
        virtual Object &setSize(const Dimensions&);
        virtual Dimensions getSize() const;
        virtual Position getPosition() const;
        virtual Object &move(Position);
        virtual Object &scale(jml::Vector2d);
        virtual Object &setTexture(const char*);
        virtual GLuint *getTexture() const;
        virtual Object &setMaterial(const Material&);
        virtual ~Object();
        friend void display();

    protected:

        virtual jutil::Queue<jutil::Queue<float> > genVAO() = 0;
        Object &formShape();

        Position position;
        Dimensions size;
        Color color;

    private:
        Object();
        bool hasTexture;
        uint32_t texture, vao;
        Position initP;
        ILuint image;
        jml::Angle rotation;
        void readyDrawSequence();
        size_t vertexCount;
        void draw();
        bool formed;
        Object &generateMVP();
        jml::Matrix<float, 4, 4> mvp;
        unsigned components;
        jml::Vector<float, 3> norm;
        jutil::Queue<jutil::Queue<float> > polygon;
        Material material;
    };
}

#endif // OBJECT_H
