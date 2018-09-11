#include "jgl.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3native.h>

namespace jgl {

    const char *FRAGMENT_SHADER = R"glsl(
        #version 440 core

        #define M_PI 3.1415926535897932384626433832795

        in vec2 jglTexCoords[16];
        vec2 texCoords;

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
        uniform sampler2D texture0;
        uniform sampler2D texture1;
        uniform sampler2D texture2;
        uniform sampler2D texture3;
        uniform sampler2D texture4;
        uniform sampler2D texture5;
        uniform sampler2D texture6;
        uniform sampler2D texture7;
        uniform sampler2D texture8;
        uniform sampler2D texture9;
        uniform sampler2D texture10;
        uniform sampler2D texture11;
        uniform sampler2D texture12;
        uniform sampler2D texture13;
        uniform sampler2D texture14;
        uniform sampler2D texture15;

        vec4 fromTexture(uint id, vec2 coord) {
            switch(id) {
                case 0u: return texture2D(texture0, coord);
                case 1u: return texture2D(texture1, coord);
                case 2u: return texture2D(texture2, coord);
                case 3u: return texture2D(texture3, coord);
                case 4u: return texture2D(texture4, coord);
                case 5u: return texture2D(texture5, coord);
                case 6u: return texture2D(texture6, coord);
                case 7u: return texture2D(texture7, coord);
                case 8u: return texture2D(texture8, coord);
                case 9u: return texture2D(texture9, coord);
                case 10u: return texture2D(texture10, coord);
                case 11u: return texture2D(texture11, coord);
                case 12u: return texture2D(texture12, coord);
                case 13u: return texture2D(texture13, coord);
                case 14u: return texture2D(texture14, coord);
                case 15u: return texture2D(texture15, coord);
            }
        }

        ///MAX 128 lights in scene
        uniform LightSource lights[128];

        layout (std140, binding = 0) uniform JGLVertexDrawData {
            float jglObjectRotation;
            vec2 jglWindowSize;
            vec2 jglObjectPosition;
            vec2 jglObjectSize;
            vec2 jglObjectTexturedArea;
            vec2 jglObjectOrigin;
            vec3 jglCameraPosition;
        };

        layout (std140, binding = 1) uniform JGLFragmentDrawData {
            uint jglTextureCount;
            int jglLightingMode;
            int jglLightCount;
            vec4 jglObjectColor;
            vec3 jglCameraNormal;
        };
        vec4 jglFragmentApplyLighting(vec4 initColor, int lightingMode) {

            float rand = gl_FragCoord.x * 353.0 + gl_FragCoord.y * 769.0;
            float noise = fract(rand / 991.0) / 256.0;
            vec3 begColor;
            begColor.rgb = vec3(0.0, 0.0, 0.0);
            float ambience = 0.02;
            vec3 ambientLight = ambience * material.ambient;
            for (int i = 0; i < jglLightCount; ++i) {

                vec3 passColor;
                passColor.rgb = vec3(initColor.r, initColor.g, initColor.b);
                vec3 fc = vec3(gl_FragCoord.x - (jglWindowSize.x / 2.0), gl_FragCoord.y - (jglWindowSize.y / 2.0), gl_FragCoord.z);

                vec3 lightPos = vec3(lights[i].pos, 1.0f);
                lightPos.x += jglCameraPosition.x;
                lightPos.y += jglCameraPosition.y;
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
                vec3 diffuse = lc * (attenuation * initColor.rgb);

                vec3 viewDir = normalize(vec3(lightPos.x, lightPos.y, 1.0) - fc);
                //vec3 lightDir = normalize(lightPos - fc);
                //vec3 reflectDir = reflect(-lightDir, jglCameraNormal);
                float spec = pow(max(dot(viewDir, jglCameraNormal), 0.0),  max(1.0 - material.shine, 0.0));
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

        vec4 sampleTextures() {
            uint untextured = 1u;
            float light = 1.0;
            vec3 lightRGB = fromTexture(0, jglTexCoords[0]).rgb;
            for (uint i = 0; i < jglTextureCount; ++i) {
                if (jglTexCoords[i].x > -0.0001 && jglTexCoords[i].y > -0.0001 && jglTexCoords[i].x < 1.0001 && jglTexCoords[i].y < 1.0001) untextured = 0u;
                vec4 layer = fromTexture(i, jglTexCoords[i]);
                light *= 1.0 - layer.a;
                if (i > 0u) {
                    lightRGB *= 1.0 - layer.a;
                    lightRGB += layer.rgb * layer.a;
                }
            }
            if (untextured > 0u) return jglObjectColor;
            return vec4(lightRGB, 1.0 - light);
        }

        vec4 jglFragmentShader() {

            vec4 fragColor;

            if (jglTextureCount > 0u) fragColor = sampleTextures();
            else fragColor = jglObjectColor;

            if (jglLightingMode > 0) jglFragmentApplyLighting(fragColor, jglLightingMode);

            return fragColor;
        }

    )glsl";

    const char *VERTEX_SHADER = R"glsl(
        #version 440 core

        #define JGL_VERTEXMODE_MANUAL       0u
        #define JGL_VERTEXMODE_RELATIVE     1u
        #define JGL_VERTEXMODE_ABSOLUTE     2u

        #define JGL_SIZE_CONTROLLER         0u
        #define EXTERNAL_SIZE_CONTROLLER    1u

        layout(location=0) in vec4 jglVertexCoordInput;
        layout(location=1) in vec2 jglTexCoordInput;

        out vec2 jglTexCoords[16];
        out vec4 jglCoords;

        struct TextureLayer {
            vec2 position;
            vec2 size;
            vec2 imageSize;
            vec2 factor;
            uvec2 controller;
            float rotation;
            uint vertexMode;
        };

        layout (std140, binding = 0) uniform JGLVertexDrawData {
            float jglObjectRotation;
            vec2 jglWindowSize;
            vec2 jglObjectPosition;
            vec2 jglObjectSize;
            vec2 jglObjectTexturedArea;
            vec2 jglObjectOrigin;
            vec3 jglCameraPosition;
        };

        layout (std140, binding = 1) uniform JGLFragmentDrawData {
            uint jglTextureCount;
            int jglLightingMode;
            int jglLightCount;
            vec4 jglObjectColor;
            vec3 jglCameraNormal;
        };

        layout (std140, binding = 2) uniform TextureDrawData {
            TextureLayer layers[16];
        };

        vec4 jglGetVertexInput() {
            return jglVertexCoordInput;
        }

        vec2 worldToTexture(vec2 vA) {
            return vec2((vA.x / 2.0f) + 0.5f, ((vA.y / 2.0f) + 0.5f) + (vA.y * -1.0f));
        }

        vec2 textureToWorld(vec2 vA) {
            return vec2((vA.x * 2.0f) - 1.0f, ((vA.y * 2) - 1) * -1);
        }

        float jglGetAspectRatio() {
            return jglWindowSize.x / jglWindowSize.y;
        }

        vec2 pixelsToScreen(vec2 pixels, vec2 screen) {
            return vec2(pixels.x / screen.x, pixels.y / screen.y);
        }

        vec2 screenToPixels(vec2 screen, vec2 window) {
            return vec2(screen.x * window.x, screen.y * window.y);
        }

        mat4 jglTranslationMatrix(vec2 offset, vec2 cameraTranslation, vec2 screenSize) {

            float yRatio = screenSize.y / (screenSize.x / 2.0);

            return mat4(
                vec4(1, 0, 0, (offset.x + cameraTranslation.x) * 2),
                vec4(0, 1, 0, -(offset.y + cameraTranslation.y) * yRatio),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );

        }

        mat4 jglRotationMatrix(float rotation) {
            return mat4(
                vec4(cos(-rotation), -sin(-rotation), 0, 0),
                vec4(sin(-rotation), cos(-rotation), 0, 0),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }

        mat4 jglScalingMatrix(vec2 size) {
            return mat4(
                vec4(size.x, 0, 0, 0),
                vec4(0, size.y, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }

        mat4 jglProjectionMatrix(float aspect) {
            return mat4(
                vec4(1, 0, 0, 0),
                vec4(0, 2.0 / (aspect + aspect), 0, 0),
                vec4(0, 0, -1, 0),
                vec4(0, 0, 0, 1)
            );
        }

        vec4 jglTransformVertex(vec4 vertex, mat4 projection, mat4 scaling, mat4 rotation, mat4 translation) {

            vec2 origin = -pixelsToScreen(jglObjectOrigin * (jglObjectSize / 2.0), jglWindowSize);

            return vertex * projection * scaling * jglTranslationMatrix(origin, vec2(0), jglWindowSize) * rotation * translation;
        }

        float getAspectRatio(vec2 screen) {
            return screen.x / screen.y;
        }

        vec2 jglGetTextureCoordinates(uint id) {

            vec2 rawImageSize   = layers[id].imageSize;
            vec2 setSize        = layers[id].size;
            vec2 objectSize     = jglObjectSize;
            vec2 scaler         = layers[id].factor;
            vec2 textureArea    = jglObjectTexturedArea;
            vec2 size           = vec2(1.0);
            vec2 sizeRatio      = rawImageSize / objectSize;

            float rotation  = layers[id].rotation;
            float oAspect   = getAspectRatio(objectSize);
            vec2 position   = layers[id].position;

            uint mode               = layers[id].vertexMode;
            uvec2 sizeController    = layers[id].controller;

            vec2 transformedVertex;
            vec2 vertex;

            if (mode == JGL_VERTEXMODE_MANUAL) {
                size = textureArea;
                vertex = textureToWorld(jglTexCoordInput);
            } else if (mode == JGL_VERTEXMODE_RELATIVE) {
                size = objectSize;
                vertex = jglVertexCoordInput.xy;
            } else if (mode == JGL_VERTEXMODE_ABSOLUTE) {
                size = rawImageSize;
                vertex = jglVertexCoordInput.xy * sizeRatio;
            }

            if (sizeController[0] == JGL_SIZE_CONTROLLER) {
                setSize[0] = size[0];
            }

            if (sizeController[1] == JGL_SIZE_CONTROLLER) {
                setSize[1] = size[1];
            }

            vec2 factor             = setSize / size;
            vec2 finalScaler        = pixelsToScreen(size * factor * scaler, objectSize);
            vec2 finalTranslater    = pixelsToScreen(vec2(position.x * -2, position.y * 2), objectSize);

            vec2 angle = vec2(cos(rotation), sin(rotation));

            mat2 rotateV = mat2(
                vec2(angle.x, -angle.y),
                vec2(angle.y, angle.x)
            );

            mat2 scaleV = mat2(
                vec2(1.0f / finalScaler.x, 0),
                vec2(0, 1.0f / finalScaler.y)
            );

            transformedVertex = (vertex + finalTranslater) * scaleV * rotateV;

            return worldToTexture(transformedVertex);
        }

        vec4 jglVertexShader() {

            jglCoords = jglTransformVertex(
                jglVertexCoordInput,
                jglProjectionMatrix(jglGetAspectRatio()),
                jglScalingMatrix(pixelsToScreen(jglObjectSize, jglWindowSize)),
                jglRotationMatrix(jglObjectRotation),
                jglTranslationMatrix(pixelsToScreen(jglObjectPosition, jglWindowSize), pixelsToScreen(jglCameraPosition.xy, jglWindowSize), jglWindowSize)
            );

            for (uint i = 0u; i < jglTextureCount; ++i) jglTexCoords[i] = jglGetTextureCoordinates(i);

            return jglCoords;
        }
    )glsl";

    const char *VERTEX_MAIN = R"glsl(
        #version jgl vertex

        void main() {
            gl_Position = jglVertexShader();
        }
    )glsl";

    const char *FRAGMENT_MAIN = R"glsl(
        #version jgl fragment

        ///resulting color of fragment
        layout(location = 0) out vec4 color;

        void main() {
            color = jglFragmentShader();
        }
    )glsl";

    jutil::Queue<LightSource> lights;

    ///variables
    jutil::String name;
    jml::Vector2u dimensions;
    jml::Vector2i position;
    GLFWwindow *win;
    Window *window = NULL;
    bool opens = true;
    Shader defaultShader;
    Position cameraPos = {0, 0};
    Core *core = NULL;
    long double fTime, fTimeLim = 0;
    jutil::Timer timer;
    GLuint shaderF, shaderV;

    GLFWwindow *getWindowHandle() {
        return win;
    }

    void pause() {
        jutil::sleep(jml::max(fTimeLim - timer.get(jutil::MILLISECONDS), 0.L));
        fTime = timer.stop(jutil::NANOSECONDS);
        timer.start();
    }

    bool keyPressed(Event::Key k) {
        return (glfwGetKey(win, (int)k) == GLFW_PRESS);
    }

    void keyHandle(GLFWwindow*, int c, int, int a, int m) {
        if (core) {
            Event e(Event::KEY, c, (Event::Action)a, m);
            core->eventHandler(e);
        }
    }

    void mouseHandle(GLFWwindow*, int b, int a, int m) {
        if (core) {
            Event e(Event::MOUSE, b, (Event::Action)a, m);
            core->eventHandler(e);
        }
    }

    void scrollHandle(GLFWwindow*, double x, double y) {
        if (core) {
            Event e(Event::SCROLL, x, y);
            core->eventHandler(e);
        }
    }

    void cursorPositionHandle(GLFWwindow*, double x, double y) {
        if (core) {
            Event e(Event::CURSOR, x, y);
            core->eventHandler(e);
        }
    }

    void closeHandle(GLFWwindow*) {
        if (core) core->eventHandler(Event(Event::CLOSE));
    }

    void sizeHandle(GLFWwindow*, int w, int h) {
        if (core) {
            Event e(Event::RESIZE, w, h);
            core->eventHandler(e);
        }
    }

    void posHandle(GLFWwindow*, int x, int y) {
        if (core) {
            Event e(Event::MOVE, static_cast<double>(x), static_cast<double>(y));
            core->eventHandler(e);
        }
    }

    void focusHandle(GLFWwindow*, int f) {
        if (core) {
            Event::Type id;
            if (f) id = Event::GAIN_FOCUS;
            else id = Event::LOSE_FOCUS;

            Event e(id);
            core->eventHandler(e);
        }

    }

    void cursorFocusHandle(GLFWwindow*, int f) {
        if (core) {
            Event::Type id;
            if (f) id = Event::CURSOR_ENTER;
            else id = Event::CURSOR_LEAVE;

            Event e(id);
            core->eventHandler(e);
        }

    }

    void init(unsigned width, unsigned height, const char *title, int pX, int pY) {

        dimensions[0] = width;
        dimensions[1] = height;

        name = title;
        glewExperimental = GL_TRUE;
        if (!glfwInit()) {
            core->errorHandler(1, "GLFW failed to initialize");
        }

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        win = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!win) {
            core->errorHandler(2, "Window creation failed");
        }
        glfwMakeContextCurrent(win);

        GLenum lerr = glewInit();
        if (lerr != GLEW_OK) {
            core->errorHandler(3, "GLEW failed to initialize");
        }

        int nModes = 0;
        const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &nModes);

        if (pX == JGL_POS_MIDDLE) {for (int i = 0; i < nModes; ++i) {if (modes[i].width > position[0]) position[0] = (modes[i].width / 2) - (width / 2);}}
        else position[0] = pX;
        if (pY == JGL_POS_MIDDLE) {for (int i = 0; i < nModes; ++i) {if (modes[i].height > position[1]) position[1] = (modes[i].height / 2) - (height / 2);}}
        else position[1] = pY;

        glfwSetKeyCallback(win, keyHandle);
        glfwSetMouseButtonCallback(win, mouseHandle);
        glfwSetScrollCallback(win, scrollHandle);
        glfwSetCursorPosCallback(win, cursorPositionHandle);
        glfwSetWindowCloseCallback(win, closeHandle);
        glfwSetWindowSizeCallback(win, sizeHandle);
        glfwSetWindowPosCallback(win, posHandle);
        glfwSetWindowFocusCallback(win, focusHandle);
        glfwSetCursorEnterCallback(win, cursorFocusHandle);

        shaderF = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shaderF, 1, &FRAGMENT_SHADER, NULL);
        shaderV = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shaderV, 1, &VERTEX_SHADER, NULL);

        window = new Window(dimensions);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glCompileShader(shaderF);
        glCompileShader(shaderV);

        glEnable(GL_MULTISAMPLE);

        defaultShader = Shader(jutil::String(VERTEX_MAIN), jutil::String(FRAGMENT_MAIN));
        setVsyncEnabled(false);

        glfwSetWindowPos(win, position[0], position[1]);

        glPolygonMode(GL_FRONT, GL_FILL);

        glViewport(0.0f, -(((float)width - (float)height) / 2.0f), (float)width, (float)height * ((float)width / (float)height));

        initializeScreens();
        timer.start();

    }

    void setVsyncEnabled(bool e) {
        if (e) glfwSwapInterval(-1);
        else glfwSwapInterval(0);
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

    void setFrameTimeLimit(long double lim) {
        fTimeLim = lim;
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
        r[1] = (-posY * winH / yRatio) + (winW - winH);

        r[0] -= cameraPos[0];
        r[1] -= cameraPos[1];

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

/*vec2 startingCoords;
            vec2 texSize;

            if (layers[id].vertexMode == JGL_VERTEXMODE_MANUAL) {
                startingCoords = textureToWorld(jglTexCoordInput);
                texSize = jglObjectSize.xy; // CALC TEXTURE AREA
            } else if (layers[id].vertexMode == JGL_VERTEXMODE_RELATIVE) {
                startingCoords = jglVertexCoordInput.xy;
            } else if (layers[id].vertexMode == JGL_VERTEXMODE_ABSOLUTE) {
                startingCoords = jglVertexCoordInput.xy;
            }

            vec2 transformedTextureVertex;

            vec2 ratio = jglObjectSize / layers[id].size;

            float oAspect = jglObjectSize.x / jglObjectSize.y;

            vec2 texFactor = texSize / jglObjectSize;

            mat2 rotateV = mat2(
                vec2(cos(layers[id].rotation), -sin(layers[id].rotation)),
                vec2(sin(layers[id].rotation), cos(layers[id].rotation))
            );

            mat2 scaleV = mat2(
                vec2(1.0f / texFactor.x, 0),
                vec2(0, 1.0f / texFactor.y)
            );

            mat2 projectV = mat2(
                vec2(1.0f, 0),
                vec2(0, 2.0f / (oAspect + oAspect))
            );

            transformedTextureVertex = (startingCoords + pixelsToScreen(vec2(layers[id].position.x * -2, layers[id].position.y * 2), jglObjectSize)) * (projectV * rotateV) * scaleV;

            transformedTextureVertex.y /= (2.0f / (oAspect + oAspect));

            vec2 conversion = worldToTexture(vec2(transformedTextureVertex.x, transformedTextureVertex.y));
            return conversion;*/
