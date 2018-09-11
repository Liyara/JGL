#ifndef JGL_H
#define JGL_H

#include "Dependencies.h"

#include "Core.h"
#include "Object.h"
#include "Quad.h"
#include "Triangle.h"
#include "Poly.h"
#include "Ellipse.h"
#include "Text.h"
#include "Line.h"
#include "Shader.h"

#define JGL_POS_MIDDLE -0xffa012e

namespace jgl {

    ///starts JGL, and opens a window
    ///@param size of window
    ///@param title of window
    void init(unsigned, unsigned, const char*, int = JGL_POS_MIDDLE, int = JGL_POS_MIDDLE);

    void begin(Core&);

    ///closes JGL's window and does some cleanup
    ///@param return code
    ///@return parameter
    int end(int);

    Core *getCore();

    void pollEvents();

    void pause();

    ///returns true if the window should not close.
    bool open();

    ///return mouse position in world coords
    Position getMouseInWorld();

    bool keyPressed(Event::Key k);

    void setMouseVisible(bool);

    long double getFrameTime(unsigned);
    void setFrameTimeLimit(long double);

    Shader getDefaultShader();
    void setDefaultShader(const Shader&);

    GLuint getDefaultFragmentShader();
    GLuint getDefaultVertexShader();

    void setVsyncEnabled(bool);

    GLFWwindow *getWindowHandle();

}

#endif // JGL_H
