#include "jgl.h"
#include "Matrix.h"
#include <Maths.h>

namespace jgl {

    jutil::Timer t;

    const jutil::Queue<const char*> UNIFORM_NAMES = {
        "material.ambient",     // 0
        "material.diffuse",     // 1
        "material.specular",    // 2
        "maetrial.shine",       // 3
        "mapTexture"            // 4
    };

    GLuint uboF, uboV, uboT, uboTP;
    unsigned fUniformIndex, vUniformIndex, tUniformIndex, sActiveP = 0, tpUniformIndex;
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
        uint32_t channel;       // 48 -> 52
        uint32_t __padding_0_;  // 52 -> 56
        uint32_t __padding_1_;  // 56 -> 60
        uint32_t __padding_2_;  // 60 -> 64
    };

    struct Object::FragmentDrawData {
        uint32_t textureCount;  // 0  -> 4
        int32_t mode;           // 4  -> 8
        int32_t lightCount;     // 8  -> 12
        uint32_t blendingMode;  // 12 -> 16
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
        TextureLayerData layers[0x30];   // 0 -> 3072
        uint32_t sep[0xc];              // 3072 -> 3120
        uint32_t link[0xc];            // 3120 -> 3168
        uint32_t connect[0xc];        // 3168 -> 3216
    } tDrawData;

    struct TexturePool {
        Image::Handle textures[0x60];   // 0 -> 6144
    } tTexturePool;

    #define INIT_UBO(ubo, data, index) \
        glGenBuffers(1, &ubo);\
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);\
        glBufferData(GL_UNIFORM_BUFFER, sizeof(data), NULL, GL_DYNAMIC_DRAW);\
        glBindBuffer(GL_UNIFORM_BUFFER, 0);\
        glBindBufferRange(GL_UNIFORM_BUFFER, index, ubo, 0, sizeof(data));

    Object::Object(const Position &p, const Dimensions &d, const Color &c) : Transformable(p, d, jml::Angle(0)), Resource(ResourceType::BUFFER),
        color(c),
        formed(false),
        components(0),
        material(),
        outline(0),
        fbo(0),
        fill(true),
        outlineColor(c),
        singleTexture(NULL),
        multiTexture(NULL),
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
                for (size_t i = 0; i < 0xc; ++i) {
                    tDrawData.sep[i] = 0;
                    tDrawData.link[i] = 0;
                    tDrawData.connect[i] = 0;
                }
                INIT_UBO(uboF, fDrawData, fUniformIndex);
                INIT_UBO(uboV, vDrawData, vUniformIndex);
                INIT_UBO(uboT, tDrawData, tUniformIndex);
                INIT_UBO(uboTP, tTexturePool, tpUniformIndex);
                initUniforms = true;
            }
        }

    Object::Object() : Object({0, 0}, {0, 0}, Color::White) {}

    Object::Object(const Object &o) : Transformable(o.position, o.size, o.rotation), Resource(o._type, o._id),
        color(o.color),
        formed(true),
        components(o.components),
        material(o.material),
        outline(o.outline),
        fbo(o.fbo),
        fill(o.fill),
        drawMode(o.drawMode),
        origin(o.origin),
        outlineColor(o.outlineColor),
        vertexCount(o.vertexCount),
        tMode(o.tMode),
        singleTexture(o.singleTexture),
        multiTexture(o.multiTexture),
        valid(o.valid) {
            if (!o.formed) getCore()->errorHandler(0xffff, "Attempt to copy ill-formed object!");
            useShader(o.shader);
            acquire();
        }


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

    void loadTextureUnit(TextureLayer *l, size_t unit) {
        if (l->getSize()[0] == JGL_AUTOMATIC_SIZE) {
            tDrawData.layers[unit].size[0] = l->getImageSize()[0];
            tDrawData.layers[unit].controller[0] = 0u;
        }
        else {
            tDrawData.layers[unit].size[0] = l->getSize()[0];
            tDrawData.layers[unit].controller[0] = 1u;
        }

        if (l->getSize()[1] == JGL_AUTOMATIC_SIZE) {
            tDrawData.layers[unit].size[1] = l->getImageSize()[1];
            tDrawData.layers[unit].controller[1] = 0u;
        }
        else {
            tDrawData.layers[unit].size[1] = l->getSize()[1];
            tDrawData.layers[unit].controller[1] = 1u;
        }

        tDrawData.layers[unit].position[0] = l->getPosition()[0];
        tDrawData.layers[unit].position[1] = l->getPosition()[1];

        tDrawData.layers[unit].imageSize[0] = l->getImageSize()[0];
        tDrawData.layers[unit].imageSize[1] = l->getImageSize()[1];

        tDrawData.layers[unit].factor[0] = l->getScalingFactor()[0];
        tDrawData.layers[unit].factor[1] = l->getScalingFactor()[1];

        tDrawData.layers[unit].channel = static_cast<uint32_t>(l->getSlot());

        tDrawData.layers[unit].rotation = static_cast<float>(static_cast<long double>(l->getRotation()));

        tDrawData.layers[unit].mode = static_cast<uint32_t>(l->getScalingMode());
    }

    void loadTextureUnit(Texture *l, size_t unit) {
        tDrawData.layers[unit].size[0] = l->getSize()[0];
        tDrawData.layers[unit].controller[0] = 0u;
        tDrawData.layers[unit].size[1] = l->getSize()[1];
        tDrawData.layers[unit].controller[1] = 0u;
        tDrawData.layers[unit].mode = 0u;

        tDrawData.layers[unit].position[0] = 0.f;
        tDrawData.layers[unit].position[1] = 0.f;

        tDrawData.layers[unit].imageSize[0] = 0.f;
        tDrawData.layers[unit].imageSize[1] = 0.f;

        tDrawData.layers[unit].factor[0] = 1.f;
        tDrawData.layers[unit].factor[1] = 1.f;

        tDrawData.layers[unit].rotation = 0.f;
    }

    void Object::render(const Screen *screen) {

        if (fbo != screen->buffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, screen->buffer);
            fbo = screen->buffer;
        }

        sActive = Shader::getActive();
        sActiveP = (sActive? sActive->id() : 0);

        if (sActiveP != shader->id()) Shader::setActive(shader);
        if (!formed) formShape();

        if (singleTexture && !multiTexture) fDrawData.textureCount = 1;
        else fDrawData.textureCount = 0;

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

        if (multiTexture) {
            size_t textureLoadCount = 0;
            size_t activeSegment = 0;
            for (size_t i = 0; i < CompositeTexture::availableSlots(); ++i) {
                if ((*multiTexture)[i].getTexture() && (*multiTexture)[i].getTexture()->handle()) {
                    tTexturePool.textures[textureLoadCount * 2] = (*multiTexture)[i].getTexture()->handle();
                    loadTextureUnit(&(*multiTexture)[i], textureLoadCount);
                    ++textureLoadCount;
                }
                if ((i + 1) % 4 == 0) {
                    tDrawData.link[activeSegment] = multiTexture->getBlender(activeSegment, PHASE_0);
                    tDrawData.connect[activeSegment] = multiTexture->getBlender(activeSegment, PHASE_1);
                    tDrawData.sep[activeSegment] = textureLoadCount;
                    ++activeSegment;
                }
            }

            fDrawData.textureCount = textureLoadCount;
        } else {
            if (singleTexture) {
                fDrawData.textureCount = 1;
                tTexturePool.textures[0] = singleTexture->handle();
                loadTextureUnit(singleTexture, 0);
            } else fDrawData.textureCount = 0;
        }

        if (fDrawData.textureCount) {
            UPDATE_UBO(uboT, tDrawData);
            UPDATE_UBO(uboTP, tTexturePool);
        }

        UPDATE_UBO(uboF, fDrawData);
        UPDATE_UBO(uboV, vDrawData);

        glBindBuffer(GL_ARRAY_BUFFER, _id);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, components * sizeof(float), NULL);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, components * sizeof(float), (char*)0 + 4 * sizeof(float));

        if (fill) glDrawArrays(drawMode, 0, vertexCount);

        if (outline) {
            glLineWidth(outline);
            glPolygonMode(GL_FRONT, GL_LINE);
            glDrawArrays(GL_POLYGON, 0, vertexCount);
            glLineWidth(1);
        }
    }

    Object &Object::setTexture(Texture *t) {
        singleTexture = t;
        return *this;
    }

    Object &Object::setTexture(CompositeTexture *c) {
        multiTexture = c;
        return *this;
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
        release();
    }

    Resource &Object::generate() {
        glGenBuffers(1, &_id);
        return *this;
    }

    Resource &Object::destroy() {
        glDeleteBuffers(1, &_id);
        return *this;
    }

    Object &Object::formShape() {
        polygon = generateVertices();
        textureObject = generateTextureVertices();

        components = 6;
        vertexCount = polygon.size();

        generate();
        acquire();

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

        glBindBuffer(GL_ARRAY_BUFFER, _id);

        glBufferData(GL_ARRAY_BUFFER, vertexCount * components * sizeof(float), &(unpackedPolygon[0]), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);

        glEnableVertexAttribArray(1);

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

    Object &Object::useShader(Shader *s) {
        shader = s;
        uniforms.clear();
        uniforms.reserve(UNIFORM_NAMES.size());
        for (auto &i: UNIFORM_NAMES) uniforms.insert(shader->getUniformID(i));
        jutil::String tNameBase = "texture", tName;
        for (size_t i = 0; i < _JGL_TEXTURE_SEGMENT_LENGTH; ++i) {
            tName = tNameBase + jutil::String(i);
            char tNameC[tName.size() + 1];
            tName.array(tNameC);
            unsigned nameID = shader->getUniformID(tNameC);
            uniforms.insert(nameID);
            shader->setUniform(nameID, static_cast<int>(i));
        }

        fUniformIndex = glGetUniformBlockIndex(shader->id(), "JGLFragmentDrawData");
        vUniformIndex = glGetUniformBlockIndex(shader->id(), "JGLVertexDrawData");
        tUniformIndex = glGetUniformBlockIndex(shader->id(), "TextureDrawData");
        tpUniformIndex = glGetUniformBlockIndex(shader->id(), "TexturePool");
        glUniformBlockBinding(shader->id(), fUniformIndex, 0);
        glUniformBlockBinding(shader->id(), vUniformIndex, 1);
        glUniformBlockBinding(shader->id(), tUniformIndex, 2);
        glUniformBlockBinding(shader->id(), tpUniformIndex, 3);
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
