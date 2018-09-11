#include "Window.h"
#include "jgl.h"

namespace jgl {

    const char *SCREEN_FRAG = R"glsl(
        #version jgl fragment
        uniform sampler2D sTex;

        out vec4 sFColor;

        in vec2 sVertices;

        void main() {
            sFColor = texture2D(sTex, sVertices);
            //sFColor = vec4(0.5, 0.4, 0.3, 1.0);
        }
    )glsl";

    const char *SCREEN_VERT = R"glsl(
        #version jgl vertex
        layout(location = 0) in vec3 screenVerts;
        layout(location = 1) in vec2 sTexVertices;

        out vec2 sVertices;

        uniform vec2 wSize;
        uniform vec2 rawPosition;

        void main() {

            float yRatio = wSize.y / (wSize.x / 2.0);

            vec2 offset = vec2(rawPosition.x / wSize.x, rawPosition.y / wSize.y);

            mat4 translateM = mat4(
                vec4(1, 0, 0, (offset.x) * 2),
                vec4(0, 1, 0, -(offset.y) * yRatio),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );

            sVertices = sTexVertices;
            gl_Position = vec4(screenVerts, 1.0) * translateM;
        }
    )glsl";

    Shader shader;
    //Window *window = NULL;

    void initializeScreens() {
        shader = Shader(jutil::String(SCREEN_VERT), jutil::String(SCREEN_FRAG));
        //window = getWindow();
    }

    Screen::Screen() : Renderable(), Translatable(0), Scalable(0), lightingMode(-1) {}

    Screen::Screen(const Position &p, const Dimensions &d) : Renderable(), Translatable(p), Scalable(d), lightingMode(-1) {
        glGenFramebuffers(1, &buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, buffer);

        glGenTextures(1, &bTexture);
        glBindTexture(GL_TEXTURE_2D, bTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d[0], d[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bTexture, 0);

        auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if(err != GL_FRAMEBUFFER_COMPLETE) {
            if (getCore()) getCore()->errorHandler(0xaf, "Error building screen buffer!");
        }

        glViewport(0.f, -(((float)d[0] - (float)d[1]) / 2.0f), (float)d[0], (float)d[1] * ((float)d[0] / (float)d[1]));



        glEnable(GL_MULTISAMPLE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        /*float aspect = static_cast<float>(window->size.x()) / static_cast<float>(window->size.y());

        float xFactor = ((static_cast<float>(d[0]) / static_cast<float>(window->size[0])));
        float yFactor = (static_cast<float>(d[1]) / static_cast<float>(window->size[1])) * (2.f / (aspect + aspect));

        vertices = {
            -xFactor, -yFactor, 0.f, 0.f, 0.f,
             xFactor, -yFactor, 0.f, 1.f, 0.f,
             xFactor, yFactor, 0.f, 1.f, 1.f,
             -xFactor, yFactor, 0.f, 0.f, 1.f
        };*/



        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), &(vertices[0]), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    Screen::~Screen() {

    }

    void Screen::render() {

        /*glBindFramebuffer(GL_FRAMEBUFFER, window->buffer);

        glViewport(0.f, -(((float)window->size[0] - (float)window->size[1]) / 2.0f), (float)window->size[0], (float)window->size[1] * ((float)window->size[0] / (float)window->size[1]));

        Shader::setActive(&shader);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bTexture);

        glEnableVertexAttribArray(1);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (char*)0 + 3 * sizeof(float));

        shader.setUniform("wSize", static_cast<jml::Vector2f>(window->size));
        shader.setUniform("rawPosition", position);

        glDrawArrays(GL_QUADS, 0, 4);

        Shader::setActive(NULL);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    }

    void Screen::setClearColor(const Color &color) {
        clearColor = color;
    }

    void Screen::clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, buffer);
        auto cN = clearColor.normals();
        glClearColor(cN.r(), cN.g(), cN.b(), cN.a());
        glClear(GL_COLOR_BUFFER_BIT);
        lights.clear();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Screen::setCameraPosition(const Position &p) {
        cameraPosition = p;
    }
    void Screen::moveCamera(const Position &p) {
        cameraPosition = cameraPosition + p;
    }
    const Position &Screen::getCameraPosition() const {
        return cameraPosition;
    }
    int Screen::lighting() const {
        return lightingMode;
    }

    void Screen::setLightingMode(int m) {
        lightingMode = m;
    }

    void Screen::useLightSource(const LightSource &l) {
        lights.insert(l);
    }

    jutil::Queue<LightSource> Screen::getLightsInScene() const {
        return lights;
    }

    Texture Screen::asTexture() const {
        //uint32_t *
        //Texture t()
    }

}
