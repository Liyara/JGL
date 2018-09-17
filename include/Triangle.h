#ifndef TRIANGLE_H
#define TRIANGLE_H

namespace jgl {
    struct Triangle : public Object {
    public:

        enum Type {
            STANDARD,
            RIGHT
        };

        Triangle(const Position&, const Dimensions&, const Color& = Color::White);
        Triangle(const Position&, const Dimensions&, Type);
        Triangle(const Position&, const Dimensions&, Type, const Color&);
        Triangle(const Position&, const Dimensions&, const Color&, Type);
        long double area() const;

    private:
        Type type;
        Triangle();
        jutil::Queue<jml::Vertex> generateVertices() const override;
    };
}

#endif // TRIANGLE_H
