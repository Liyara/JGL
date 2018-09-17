#include "jgl.h"
#include "Matrix.h"
#include <Maths.h>

namespace jgl {

    const jutil::Queue<const char*> UNIFORM_NAMES = {
        "material.ambient",     // 0
        "material.diffuse",     // 1
        "material.specular",    // 2
        "maetrial.shine"        // 3
    };

    GLuint uboF, uboV, uboT;
    unsigned fUniformIndex, vUniformIndex, tUniformIndex, sActiveP = 0;
    GLvoid *uboData = NULL;
    Shader *sActive = NULL;
    jml::Vector4f cNorms;
    GLbitfield mapping = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
    bool initUniforms = false;

    struct TextureLayerData {
        float position[2];      // 0 -> 8
        float size[2];          // 8 -> 16
        float imageSize[2];     // 16 -> 24
        float factor[2];        // 24 -> 32
        unsigned controller[2]; // 32 -> 40
        float rotation;         // 40 -> 44
        unsigned mode;          // 44 -> 48
    };

    struct Object::FragmentDrawData {
        uint32_t textureCount;  // 0  -> 4
        int32_t mode;           // 4  -> 8
        int32_t lightCount;     // 8  -> 12
        uint32_t __padding_0_;  // 12 -> 16
        float fcolor[4];        // 16 -> 32
        float normal[3];        // 32 -> 48
    } fDrawData;

    struct Object::VertexDrawData {
        float rotation;             // 0  -> 4
        uint32_t __padding_0_;      // 4  -> 8
        float wSize[2];             // 8  -> 16
        float rawPosition[2];       // 16 -> 24
        float rawSize[2];           // 24 -> 32
        float origin[2];            // 32 -> 40
        float cameraPosition[3];    // 40 -> 56
    } vDrawData;

    struct Object::TextureDrawData {
        TextureLayerData layers[_JGL_TEXTURE_SEGMENT_LENGTH];   // 0 -> 512
    } tDrawData;


    #define INIT_UBO(ubo, data, index) \
        glGenBuffers(1, &ubo);\
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);\
        glBufferData(GL_UNIFORM_BUFFER, sizeof(data), NULL, GL_DYNAMIC_DRAW);\
        glBindBuffer(GL_UNIFORM_BUFFER, 0);\
        glBindBufferRange(GL_UNIFORM_BUFFER, index, ubo, 0, sizeof(data));

    Object::Object(const Position &p, const Dimensions &d, const Color &c) : Transformable(p, d, jml::Angle(0)),
        color(c),
        formed(false),
        components(0),
        material(),
        outline(0),
        fbo(0),
        fill(true),
        outlineColor(c),
        drawMode(GL_POLYGON) {
            useShader(getDefaultShader());
            if (!initUniforms) {
                for (size_t i = 0; i < _JGL_TEXTURE_SEGMENT_LENGTH; ++i) {

                    tDrawData.layers[i].mode = 0u;

                    tDrawData.layers[i].position[0] = 0.f;
                    tDrawData.layers[i].position[1] = 0.f;

                    tDrawData.layers[i].size[0] = 0.f;
                    tDrawData.layers[i].size[1] = 0.f;

                    tDrawData.layers[i].imageSize[0] = 0.0f;
                    tDrawData.layers[i].imageSize[1] = 0.0f;

                    tDrawData.layers[i].factor[0] = 0.0f;
                    tDrawData.layers[i].factor[1] = 0.0f;

                    tDrawData.layers[i].controller[0] = 0u;
                    tDrawData.layers[i].controller[1] = 0u;

                    tDrawData.layers[i].rotation = 0.f;
                }
                INIT_UBO(uboF, fDrawData, fUniformIndex);
                INIT_UBO(uboV, vDrawData, vUniformIndex);
                INIT_UBO(uboT, tDrawData, tUniformIndex);
                initUniforms = true;
            }
        }

    Object::Object() : Object({0, 0}, {0, 0}, Color::White) {}

    jml::Vertex textureToWorld(jml::Vertex vA) {
        return {(vA.x() * 2.0f) - 1.0f, ((vA.y() * 2) - 1) * -1, vA.z(), vA.w()};
    }

    jml::Vertex worldToTexture(jml::Vertex vA) {
        return {(vA.x() / 2.0f) + 0.5f, ((vA.y() / 2.0f) + 0.5f) + (vA.y() * -1.0f), vA.z(), vA.w()};
    }

    #define UPDATE_UBO(ubo, data) \
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);\
        uboData = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(data), mapping);\
        if (uboData) memcpy(uboData, &data, sizeof(data));\
        glUnmapBuffer(GL_UNIFORM_BUFFER);

    void Object::render(const Screen *screen) {

        if (fbo != screen->buffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, screen->buffer);
            fbo = screen->buffer;
        }

        sActive = Shader::getActive();
        sActiveP = (sActive? sActive->getProgram() : 0);

        if (sActiveP != shader.getProgram()) Shader::setActive(&shader);
        if (!formed) formShape();

        fDrawData.textureCount = numLayers();

        auto &sSize = screen->getSize();
        auto &sPos = screen->getCameraPosition();

        fDrawData.mode = screen->lighting();
        vDrawData.wSize[0] = sSize[0];
        vDrawData.wSize[1] = sSize[1];

        vDrawData.rawSize[0] = size[0];
        vDrawData.rawSize[1] = size[1];

        vDrawData.rawPosition[0] = position[0];
        vDrawData.rawPosition[1] = position[1];

        vDrawData.rotation = static_cast<float>(static_cast<long double>(rotation));

        vDrawData.cameraPosition[0] = sPos[0];
        vDrawData.cameraPosition[1] = sPos[1];

        fDrawData.fcolor[0] = (color.red() / 255.f);
        fDrawData.fcolor[1] = (color.green() / 255.f);
        fDrawData.fcolor[2] = (color.blue() / 255.f);
        fDrawData.fcolor[3] = (color.alpha() / 255.f);

        vDrawData.origin[0] = origin[0];
        vDrawData.origin[1] = origin[1];

        if (numLayers()) {

            size_t textureLoadCount = jml::min(numLayers(), static_cast<size_t>(_JGL_TEXTURE_SEGMENT_LENGTH));

            for (size_t i = 0; i < textureLoadCount; ++i) {

                if (layers[i].getSize()[0] == JGL_AUTOMATIC_SIZE) {
                    tDrawData.layers[i].size[0] = layers[i].getImageSize()[0];
                    tDrawData.layers[i].controller[0] = 0u;
                }
                else {
                    tDrawData.layers[i].size[0] = layers[i].getSize()[0];
                    tDrawData.layers[i].controller[0] = 1u;
                }

                if (layers[i].getSize()[1] == JGL_AUTOMATIC_SIZE) {
                    tDrawData.layers[i].size[1] = layers[i].getImageSize()[1];
                    tDrawData.layers[i].controller[1] = 0u;
                }
                else {
                    tDrawData.layers[i].size[1] = layers[i].getSize()[1];
                    tDrawData.layers[i].controller[1] = 1u;
                }

                tDrawData.layers[i].mode = layers[i].getScalingMode();

                tDrawData.layers[i].position[0] = layers[i].getPosition()[0];
                tDrawData.layers[i].position[1] = layers[i].getPosition()[1];

                tDrawData.layers[i].imageSize[0] = layers[i].getImageSize()[0];
                tDrawData.layers[i].imageSize[1] = layers[i].getImageSize()[1];

                tDrawData.layers[i].factor[0] = layers[i].getScalingFactor()[0];
                tDrawData.layers[i].factor[1] = layers[i].getScalingFactor()[1];

                tDrawData.layers[i].rotation = static_cast<float>(static_cast<long double>(layers[i].getRotation()));

                glActiveTexture(_JGL_TEXTURE_SEGMENT + i);
                glBindTexture(GL_TEXTURE_2D, layers[i].getTexture()->getID());
            }

            UPDATE_UBO(uboT, tDrawData);
        }

        UPDATE_UBO(uboF, fDrawData);
        UPDATE_UBO(uboV, vDrawData);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, components * sizeof(float), NULL);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, components * sizeof(float), (char*)0 + 4 * sizeof(float));

        if (fill) {
            glDrawArrays(drawMode, 0, vertexCount);
        }

        if (outline) {
            glLineWidth(outline);
            glPolygonMode(GL_FRONT, GL_LINE);
            glDrawArrays(GL_POLYGON, 0, vertexCount);
            glLineWidth(1);
        }
    }

    void Object::render() {
        render(getWindow());
    }


    Object &Object::setMaterial(const Material &m) {
        material = m;
        return *this;
    }

    Object &Object::setColor(const Color &c) {
        color = c;
        return *this;
    }
    Color Object::getColor() const {
        return color;
    }

    Object::~Object() {

    }

    Object &Object::formShape() {
        polygon = generateVertices();
        textureObject = generateTextureVertices();

        jutil::out << textureObject << jutil::endl;

        components = 6;
        vertexCount = polygon.size();

        ///Enable use of object within OPENGL
        glGenBuffers(1, &vbo);

        loadPolygon();

        formed = true;

        return *this;
    }

    void Object::loadPolygon() {

        GLint m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);
        const float ASPECT = ((float)m_viewport[2] / (float)m_viewport[3]);

        unpackedPolygon.clear();
        unpackedPolygon.reserve(vertexCount * components);

        size_t texIndex = 0;

        for (auto &i: polygon) {
            i[0] *= ASPECT;
            for (auto &j: i) {
                unpackedPolygon.insert(j);
            }
            if (texIndex < textureObject.size()) {
                for (size_t j = 0; j < 2; ++j) {
                    unpackedPolygon.insert(textureObject[texIndex][j]);
                }
                ++texIndex;
            } else {
                for (size_t j = 0; j < 2; ++j) {
                    unpackedPolygon.insert(1.L);
                }
            }
        }

        ///Bind object to the "GL_ARRAY_BUFFER" section of the OPENGL context
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        ///allocate memory to the object we've just bound in "GLARRAY_BUFFER" in the GPU
        glBufferData(GL_ARRAY_BUFFER, vertexCount * components * sizeof(float), &(unpackedPolygon[0]), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);

        glEnableVertexAttribArray(1);

        ///unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    jutil::Queue<jml::Vertex> Object::getVertices() {

    }

    jutil::Queue<jml::Vertex> Object::generateTextureVertices() const {
        jutil::Queue<jml::Vertex> tv;
        for (auto &i: polygon) {
            tv.insert(worldToTexture(i));
        }
        return tv;
    }

    Object &Object::setOutline(uint8_t o) {
        outline = o;
        return *this;
    }
    uint8_t Object::getOutline() const {
        return outline;
    }
    Object &Object::setFill(bool f) {
        fill = f;
        return *this;
    }
    Object &Object::setOutlineColor(const Color &c) {
        outlineColor = c;
        return *this;
    }

    Object &Object::useShader(const Shader &s) {
        shader = s;
        uniforms.clear();
        uniforms.reserve(UNIFORM_NAMES.size());
        for (auto &i: UNIFORM_NAMES) uniforms.insert(shader.getUniformID(i));
        jutil::String tNameBase = "texture", tName;
        for (size_t i = 0; i < _JGL_TEXTURE_SEGMENT_LENGTH; ++i) {
            tName = tNameBase + jutil::String(i);
            char tNameC[tName.size() + 1];
            tName.array(tNameC);
            unsigned nameID = shader.getUniformID(tNameC);
            uniforms.insert(nameID);
            shader.setUniform(nameID, static_cast<int>(i));

        }
        fUniformIndex = glGetUniformBlockIndex(shader.getProgram(), "JGLFragmentDrawData");
        vUniformIndex = glGetUniformBlockIndex(shader.getProgram(), "JGLVertexDrawData");
        tUniformIndex = glGetUniformBlockIndex(shader.getProgram(), "TextureDrawData");
        glUniformBlockBinding(shader.getProgram(), fUniformIndex, 0);
        glUniformBlockBinding(shader.getProgram(), vUniformIndex, 1);
        glUniformBlockBinding(shader.getProgram(), tUniformIndex, 2);
        return *this;
    }
    Object &Object::setMode(GLenum m) {
        drawMode = m;
        return *this;
    }
    GLenum Object::getMode() const {
        return drawMode;
    }

    Object &Object::setOrigin(const jml::Vector2f &o) {
        origin = o;
        return *this;
    }
    const jml::Vector2f &Object::getOrigin() const {
        return origin;
    }


}
