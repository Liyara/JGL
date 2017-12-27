#ifndef QUAD_H
#define QUAD_H

namespace jgl {
    struct Quad : public Object {
    public:
        Quad(Position, Dimensions, Color = Color::White);
        float area();
    protected:
        jutil::Queue<jutil::Queue<float> > genVAO();
    private:
        Quad();
    };
}

#endif // QUAD_H
