#ifndef JGL_H
#define JGL_H

#include "Dependencies.h"

#include "Core.h"
#include "Object.h"
#include "Quad.h"
#include "Triangle.h"
#include "Poly.h"
#include "Ellipse.h"
#include "LightSource.h"
#include "Text.h"
#include "Line.h"
#include "Shader.h"

namespace jgl {

    ///starts JGL, and opens a window
    ///@param size of window
    ///@param title of window
    void init(unsigned, const jutil::String&);

    void begin(Core&);

    ///closes JGL's window and does some cleanup
    ///@param return code
    ///@return parameter
    int end(int);

    Core *getCore();

    ///rendering
    void clear();
    void display();
    void setClearColor(const Color&);
    void render(Object&);

    void pollEvents();

    void pause();

    ///check if lighting is enabled
    int lighting();

    ///enable/disable lighting
    void setLightingMode(int);

    ///returns uniform variable location from built-in shader program
    //uint32_t getUniform(const char*);

    ///get window size
    jml::Vector2u getWindowSize();

    ///use this lightsource during this display
    void useLightSource(LightSource);

    ///returns vector containing all in-use light sources
    jutil::Queue<LightSource> getLightsInScene();

    ///returns vector containing all in-use light objects
    jutil::Queue<Object*> getObjectsInScene();

    ///returns true if the window should not close.
    bool open();

    ///return mouse position in world coords
    Position getMouseInWorld();

    void moveCamera(float, float);
    void setCameraPosition(float, float);

    Position getCameraPosition();

    bool keyPressed(Event::Key k);

    void setMouseVisible(bool);

    long double getFrameTime(unsigned);
    void setFrameTimeLimit(long double);

    Shader getDefaultShader();
    void setDefaultShader(const Shader&);

    GLuint getDefaultFragmentShader();
    GLuint getDefaultVertexShader();

}

#endif // JGL_H
