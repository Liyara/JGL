#include "jgl.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3native.h>

namespace jgl {

    const char *FRAGMENT_SHADER = R"(
        #version 330

        #define M_PI 3.1415926535897932384626433832795

        struct LightSource {
            vec2 pos;
            float intensity;
            vec3 c;
        };

        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shine;
        };

        uniform Material material;

        ///texture in use
        uniform sampler2D tex;

        ///BOOLEAN -> object has texture?
        uniform uint hastex;

        ///BOOLEAN -> lighting is enabled?
        uniform int mode;

        ///BOOLEAN -> object is text?
        uniform uint isText;

        ///color of fragment if no texture is applied
        uniform vec4 fcolor;

        ///number of lights in scene
        uniform int lightCount;

        ///MAX 128 lights in scene
        uniform LightSource lights[128];

        ///dimensions of window
        uniform vec2 wsize;

        uniform vec3 normal;

        uniform vec3 cameraPos;

        uniform vec2 offset;

        uniform vec2 rawPosition;

        ///coordinated in texture for fragment
        in vec2 ftexcoord;

        vec4 jglFragmentAsColor() {
            return fcolor;
        }

        vec4 jglFragmentFromTexture() {
            if (isText > 0u) {
                vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(tex, ftexcoord).r);
                return fcolor * sampled;
            } else {
                return texture2D(tex, ftexcoord);
            }
        }

        uint jglFragmentIsTextured() {
            return hastex;
        }

        int jglFragmentGetLightingMode() {
            return mode;
        }

        vec4 jglFragmentApplyLighting(vec4 initColor, int lightingMode) {
            float rand = gl_FragCoord.x * 353.0 + gl_FragCoord.y * 769.0;
            float noise = fract(rand / 991.0) / 256.0;
            vec3 begColor;
            begColor.rgb = vec3(0.0, 0.0, 0.0);
            float ambience = 0.02;
            vec3 ambientLight = ambience * material.ambient;
            for (int i = 0; i < lightCount; ++i) {
                vec3 passColor;
                passColor.rgb = vec3(initColor.r, initColor.g, initColor.b);
                vec3 fc = vec3(gl_FragCoord.x - (wsize.x / 2), gl_FragCoord.y - (wsize.y / 2), gl_FragCoord.z);

                vec3 lightPos = vec3(lights[i].pos, 1.0f);
                vec3 lc = lights[i].c;

                float mR = ambience + noise;

                lc.r = max(lc.r, mR);
                lc.g = max(lc.g, mR);
                lc.b = max(lc.b, mR);


                float intensity = lights[i].intensity * 100;
                float iA = 1 / sqrt(intensity);
                float iB = 1 / intensity;
                float d = distance(lightPos, fc);
                float attenuation = 1.0 / (1.0 + (iA * d) + (iB * pow(d, 2)));
                vec3 diffuse = lc * (attenuation * material.diffuse);

                vec3 viewDir = normalize(vec3(lightPos.x, lightPos.y, 1.0) - fc);
                //vec3 lightDir = normalize(lightPos - fc);
                //vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, normal), 0.0),  max(1.0 - material.shine, 0.0));
                vec3 specular = lc * (spec * material.specular);

                vec3 finalColor;

                if (lightingMode == 2) {
                    finalColor = (specular + noise) + (diffuse + noise);
                } else if (lightingMode == 1) {
                    float t = 0.5;
                    float m1 = sqrt(pow(passColor.r, 2) + pow(passColor.g, 2) + pow(passColor.b, 2));
                    float m2 = sqrt(pow(lc.r, 2) + pow(lc.g, 2) + pow(lc.b, 2));
                    float m = sqrt(pow(m1, 2) + pow(m2, 2));
                    vec3 baseColor;
                    baseColor.rgb = vec3(
                        sqrt((1 - t) * pow(passColor.r, 2) + t * pow(lc.r, 2)) * m,
                        sqrt((1 - t) * pow(passColor.g, 2) + t * pow(lc.g, 2)) * m,
                        sqrt((1 - t) * pow(passColor.b, 2) + t * pow(lc.b, 2)) * m
                    );
                    finalColor = (baseColor * diffuse);
                }

                begColor += finalColor;
            }

            return vec4(((ambientLight + noise) + begColor), initColor.a);

        }

        vec4 jglFragmentShader() {
            vec4 initColor;

            if (hastex > 0u) initColor.rgba = jglFragmentFromTexture();
            else initColor.rgba = jglFragmentAsColor();

            if (mode != -1 && isText <= 0u) return jglFragmentApplyLighting(initColor, mode);
            else return initColor;
        }

    )";

    const char *VERTEX_SHADER = R"(
        #version 330
        layout(location=0) in vec4 position;
        layout(location=1) in vec2 texcoord;
        out vec2 ftexcoord;
        uniform vec2 offset;
        uniform mat4 mvp;
        uniform float angle;
        uniform float yRatio;

        vec4 jglVertexShader() {
            ftexcoord = texcoord;
            mat4 translation = mat4(
                vec4(1, 0, 0, offset.x * 2),
                vec4(0, 1, 0, -offset.y * yRatio),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
            return (position * mvp * translation);
        }
    )";

    jutil::Queue<LightSource> lights;
    jutil::Queue<Object*> objects;

    ///variables
    jutil::String name;
    jml::Vector2u dimensions;
    jutil::Queue<jml::Vector2f> shadowMap;
    GLFWwindow *win;
    Color clearColor;
    bool opens = true;
    int lMode = 1;
    Shader defaultShader;
    Position cameraPos = {0, 0};
    Core *core;
    Object *background;
    long double fTime, fTimeLim = 0;
    jutil::Timer timer;
    GLuint shaderF, shaderV;

    void pause() {
        jutil::sleep(jml::max(fTimeLim - timer.get(jutil::MILLISECONDS), 0.L));
        fTime = timer.stop(jutil::NANOSECONDS);
        timer.start();
    }

    bool keyPressed(Event::Key k) {
        return (glfwGetKey(win, (int)k) == GLFW_PRESS);
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

    void init(unsigned width, const jutil::String &title) {

        unsigned height = width / (16.0 / 9.0);

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
        glfwWindowHint(GLFW_SAMPLES, 16);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);


        char titlecstr[title.size() + 1];
        title.array(titlecstr);

        win = glfwCreateWindow(width, height, titlecstr, NULL, NULL);
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

        shaderF = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shaderF, 1, &FRAGMENT_SHADER, NULL);
        shaderV = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shaderV, 1, &VERTEX_SHADER, NULL);

        glCompileShader(shaderF);
        glCompileShader(shaderV);

        defaultShader = Shader("C:\\Users\\Liyara\\Documents\\Programming\\Libraries\\JGL\\shader\\main.vert", "C:\\Users\\Liyara\\Documents\\Programming\\Libraries\\JGL\\shader\\main.frag");
        glfwSwapInterval(0);
        glEnable(GL_DEPTH);
        glDepthMask(false);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glViewport(0.0f, -(((float)width - (float)height) / 2.0f), (float)width, (float)height * ((float)width / (float)height));

        clearColor = Color::White;
        background = new Quad({0, 0}, {width, unsigned(int((float)height * ((float)width / (float)height)))}, clearColor);
        background->setMaterial(Material::Rubber);
        timer.start();
    }

    GLuint getDefaultFragmentShader() {
        return shaderF;
    }
    GLuint getDefaultVertexShader() {
        return shaderV;
    }

    Shader getDefaultShader() {
        return defaultShader;
    }

    void setDefaultShader(const Shader &s) {
        defaultShader = s;
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
        core->gameLoop();
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
        if (opens) {
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
    }

    void render(Object &o) {
        if (!objects.find(&o)) objects.insert(&o);
    }

    void render(jutil::Queue<Object*> objs) {
        for (auto &i: objs) render(*i);
    }

    void setFrameTimeLimit(long double lim) {
        fTimeLim = lim;
    }

    void display() {
        if (opens) {
            for (auto &i: objects) {
                i->draw();
            }
            objects.clear();
            shadowMap.clear();
            glFlush();
            lights.clear();
            glfwSwapBuffers(win);
        }
    }
    void setClearColor(const Color &c) {
        clearColor = c;
    }

    long double getFrameTime(unsigned t) {
        long double time = fTime;
        for (size_t i = 0; i < t; ++i) {
            time /= 1000.L;
        }
        return time;
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

    jml::Vector2u getWindowSize() {
        return dimensions;
    }
    bool open() {
        if (glfwWindowShouldClose(win)) return false;
        else return opens;
    }

    void useLightSource(LightSource l) {
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

        GLdouble winW = (GLdouble)getWindowSize().x();
        GLdouble winH = (GLdouble)getWindowSize().y();


        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        GetCursorPos(&mouse);
        ScreenToClient(glfwGetWin32Window(win), &mouse);
        winX = (GLfloat)mouse.x;
        winY = (GLfloat)mouse.y;
        winY = (GLfloat)viewport[3] - winY;
        glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        GLdouble yRatio = winH / ((winW / GLdouble(2.0f)));

        r[0] = posX * winW / GLdouble(2.0);
        r[1] = (-posY * winH / yRatio) + ((winW / GLdouble(2.0)) * GLdouble(7.0 / 8.0));

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

    Position getCameraPosition() {
        return cameraPos;
    }
}
