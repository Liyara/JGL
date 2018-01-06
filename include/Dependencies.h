#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#ifndef DGLEW_STATIC
#define DGLEW_STATIC
#endif

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <Core/jutil.h>
#include <Maths.h>

/**
    The following defines are necessary
    to iron out incompatibilites between
    JUtil and parts of the C++ Standard
    Library used by other dependencies.
**/
#define _STDINT_H
#define _UNISTD_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <png.h>
#include <IL/ilut.h>

#endif // DEPENDENCIES_H
