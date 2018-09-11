#ifndef JGL_WINDOW_H
#define JGL_WINDOW_H

#include "Screen.h"

namespace jgl {
    class Window : public Screen {
    public:
        void render() override;
        virtual ~Window();
    private:
        Window(Dimensions);
        friend void init(unsigned, unsigned, const char*, int, int);
    };

    Window *const getWindow();
}



#endif // JGL_WINDOW_H
