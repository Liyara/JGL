#ifndef TRIANGLE_H
#define TRIANGLE_H

namespace jgl {
    struct Triangle : public Object {
    public:

        enum Type {
            STANDARD,
            RIGHT
        };

        Triangle(Position, Dimensions, Color = Color::White);
        Triangle(Position, Dimensions, Type);
        Triangle(Position, Dimensions, Type, Color);
        Triangle(Position, Dimensions, Color, Type);
        float area();

    private:
        Type type;
        Triangle();
        jutil::Queue<jutil::Queue<float> > genVAO();
    };
}

#endif // TRIANGLE_H
