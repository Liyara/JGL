#ifndef JGL_RENDERABLE_H
#define JGL_RENDERABLE_H

namespace jgl {

    class Renderable {
    public:

        virtual void render() = 0;

        virtual ~Renderable();

    protected:

    };
}

#endif // JGL_RENDERABLE_H
