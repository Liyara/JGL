#include "jgl.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3native.h>

namespace jgl {

    jutil::Queue<LightSource> lights;
    jutil::Queue<Object*> objects;

    GLuint createShader(GLenum eShaderType, const jutil::String &strShaderFile) {
        char strProto[strShaderFile.size() + 1];
        strShaderFile.array(strProto);
        const char *strFileData = strProto;
        strProto[strShaderFile.size()] = '\0';
        GLuint shader = glCreateShader(eShaderType);
        glShaderSource(shader, 1, &strFileData, NULL);

        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

            const char *strShaderType = NULL;
            switch(eShaderType) {
            case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
            case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
            case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
            }

            jutil::err << "Compile failure in " << strShaderType << "shader: \n" << strInfoLog << jutil::endl;
            delete[] strInfoLog;
        }

        return shader;
    }

    GLuint createProgram(const jutil::Queue<GLuint> &shaderList) {
        GLuint program = glCreateProgram();

        for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) glAttachShader(program, shaderList[iLoop]);

        glLinkProgram(program);

        GLint status;
        glGetProgramiv (program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
            jutil::err << "Link failure: " << strInfoLog << jutil::endl;
            delete[] strInfoLog;
        }

        for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) glDetachShader(program, shaderList[iLoop]);
        return program;
    }

    ///variables
    jutil::String name;
    jml::Vector2u dimensions;
    jutil::Queue<jml::Vector2f> shadowMap;
    GLFWwindow *win;
    Color clearColor;
    bool opens = true;
    int lMode = 1;
    GLuint program;
    Position cameraPos = {0, 0};
    Core *core;
    Object *background;

    void gameLoop() {
        while (open()) {
            pollEvents();

            if (!core->loop()) {
                jgl::end(0);
                break;
            }

            display();
            clear();
        }
    }

    bool keyPressed(Event::Key k) {
        return (glfwGetKey(win, (int)k) == GLFW_PRESS);
    }

    void defaultEventHandler(Event e) {
        switch(e.type) {
            case Event::KEY: {
                switch(e.code) {
                    case Event::ESCAPE: {
                        end(0);
                        break;
                    }
                    default: break;
                }
                break;
            }
            default: break;
        }
    }

    void keyHandle(GLFWwindow*, int c, int, int a, int m) {
        Event e(Event::KEY, c, (Event::Action)a, m);
        core->eventHandler(e);
    }

    void mouseHandle(GLFWwindow*, int b, int a, int m) {
        Event e(Event::MOUSE, b, (Event::Action)a, m);
        core->eventHandler(e);
    }

    void scrollHandle(GLFWwindow*, double x, double y) {
        Event e(Event::SCROLL, x, y);
        core->eventHandler(e);
    }

    void cursorPositionHandle(GLFWwindow*, double x, double y) {
        Event e(Event::CURSOR, x, y);
        core->eventHandler(e);
    }

    void closeHandle(GLFWwindow*) {
        end(0);
    }

    void init(unsigned width, const char *title) {

        unsigned height = width / (16.0d / 9.0d);

        dimensions[0] = width;
        dimensions[1] = height;
        name = title;
        glewExperimental = GL_TRUE;
        if (!glfwInit()) {
            core->errorHandler(1, "GLFW failed to initialize");
        }
        ilInit();
        iluInit();
        ilutRenderer(ILUT_OPENGL);

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_MULTISAMPLE);

        win = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!win) {
            core->errorHandler(2, "Window creation failed");
        }
        glfwMakeContextCurrent(win);

        GLenum lerr = glewInit();
        if (lerr != GLEW_OK) {
            core->errorHandler(3, "GLEW failed to initialize");
        }

        glfwSetKeyCallback(win, keyHandle);
        glfwSetMouseButtonCallback(win, mouseHandle);
        glfwSetScrollCallback(win, scrollHandle);
        glfwSetCursorPosCallback(win, cursorPositionHandle);
        glfwSetWindowCloseCallback(win, closeHandle);

        jutil::String vertex_s = "", frag_s = "", line = "";
        jutil::File vertex_f("C:\\Users\\Liyara\\Documents\\Programming\\Libraries\\JGL\\shader\\main.vert", jutil::File::Mode::READ);
        while (!vertex_f.eof()) {
            vertex_f >> line;
            vertex_s += line;
        }
        jutil::File frag_f("C:\\Users\\Liyara\\Documents\\Programming\\Libraries\\JGL\\shader\\main.frag", jutil::File::Mode::READ);
        while (!frag_f.eof()) {
            frag_f >> line;
            frag_s += line;
        }
        program = createProgram({createShader(GL_VERTEX_SHADER, vertex_s), createShader(GL_FRAGMENT_SHADER, frag_s)});
        glUseProgram(program);
        glfwSwapInterval(1);
        glEnable(GL_DEPTH);
        glViewport(0.0f, -(((float)width - (float)height) / 2.0f), (float)width, (float)height * ((float)width / (float)height));

        background = new Quad({0, 0}, {width, unsigned(int((float)height * ((float)width / (float)height)))}, jgl::Color::White);
        background->setMaterial(Material::Rubber);
        clearColor = Color::White;

    }

    void setMouseVisible(bool v) {
        if (v) {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
    }

    void begin(Core &c) {
        core = &c;
        gameLoop();
    }

    int end(int code) {
        opens = false;
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glfwSetWindowShouldClose(win, true);
        glfwTerminate();
        return code;
    }

    void clear() {
        Dimensions d = getWindowSize();
        unsigned width = d.x(), height = d.y();
        float cameraY = cameraPos.y(), cameraX = cameraPos.x();
        glViewport(0.0f + cameraX , -(((float)width - (float)height) / 2.0f) + cameraY, (float)width, (float)height * ((float)width / (float)height));
        background->setColor(clearColor);
        background->setPosition({0.0f - cameraX/ 2 , -(((float)width - (float)height) / 2.0f) - cameraY});

        glClearColor(
            Color::normal(clearColor.red()),
            Color::normal(clearColor.green()),
            Color::normal(clearColor.blue()),
            Color::normal(clearColor.alpha())
        );

        glClear(GL_COLOR_BUFFER_BIT);
    }

    void render(Object &o) {
        objects.insert(&o);
    }

    void render(jutil::Queue<Object*> objs) {
        objects.insert(objs);
    }

    void display() {
        //objects.insert(background, 0);
        for (auto &i: objects) {
            i->draw();
        }

        objects.clear();
        shadowMap.clear();
        lights.clear();
        glfwSwapBuffers(win);
    }
    void setClearColor(Color c) {
        clearColor = c;
    }

    void pollEvents() {
        glfwPollEvents();
    }

    int lighting() {
        return lMode;
    }

    void setLightingMode(int m) {
        lMode = m;
    }

    GLuint getUniform(const char* n) {
        return glGetUniformLocation(program, n);
    }

    jml::Vector2u getWindowSize() {
        return dimensions;
    }
    bool open() {
        if (glfwWindowShouldClose(win)) return false;
        else return opens;
    }

    void useLightSource(LightSource l) {
        l.position.y() *= 2;
        l.position.x() *= 2;
        lights.insert(l);
    }

    jutil::Queue<LightSource> getLightsInScene() {
        return lights;
    }

    jutil::Queue<Object*> getObjectsInScene() {
        return objects;
    }

    Position getMouseInWorld() {
        Position r;

        GLint viewport[0x04];
        GLdouble modelview[0x10];
        GLdouble projection[0x10];
        POINT mouse;
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;


        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        GetCursorPos(&mouse);
        ScreenToClient(glfwGetWin32Window(win), &mouse);
        winX = (float)mouse.x;
        winY = (float)mouse.y;
        winY = (float)viewport[3] - winY;
        glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        float yRatio = (float)getWindowSize().y() / ((float)getWindowSize().x() / 2.0f);

        posX = (posX * getWindowSize().x()) / 2.0f;
        posY = (-posY * ((float)getWindowSize().y() / yRatio)) + (((float)getWindowSize().x() / 2.0f) * (7.0f / 8.0f));

        r[0] = posX;
        r[1] = posY;

        return r;
    }


    void moveCamera(float x, float y) {
        cameraPos.x() += -x;
        cameraPos.y() += y;
    }

    void setCameraPosition(float x, float y) {
        cameraPos = {-x, y};
    }

    Core *getCore() {
        return core;
    }

    jml::Vector2f getCameraPosition() {
        return cameraPos;
    }
}
