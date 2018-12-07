#include "jgl.h"
#include "Matrix.h"
#include <Maths.h>

namespace jgl {

    jutil::Timer t;

    const jutil::Queue<const char*> UNIFORM_NAMES = {
        "material.ambient",     // 0
        "material.diffuse",     // 1
        "material.specular",    // 2
        "maetrial.shine"        // 3
    };

    GLuint uboF, uboV, uboT, uboTP;
    unsigned fUniformIndex, vUniformIndex, tUniformIndex, sActiveP = 0, tpUniformIndex;
    GLvoid *uboData = NULL;
    Shader *sActive = NULL;
    jml::Vector4f cNorms;
    GLbitfield mapping = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
    bool initUniforms = false;
    size_t textureDataSize;

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

    struct TextureSegment {
        TextureLayerData layers[4]; // 0 -> 256
        uint32_t sep;               // 256 -> 260
        uint32_t link;              // 260 -> 264
        uint32_t connect;           // 264 -> 268
        uint32_t __padding_0__;     // 268 -> 272
    };

    struct Object::TextureDrawData {
        TextureSegment segments[0xf]; // 0 -> 4352
    } tDrawData;

    struct TexturePool {
        Image::Handle *textures;
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
                fDrawData = {0};
                vDrawData = {0};
                tDrawData = {0};
                INIT_UBO(uboF, fDrawData, fUniformIndex);
                INIT_UBO(uboV, vDrawData, vUniformIndex);
                INIT_UBO(uboT, tDrawData, tUniformIndex);
                if (GLEW_ARB_bindless_texture) {
                    int numTextures = (CompositeTexture::availableSlots() + (CompositeTexture::availableSlots() / 4)) * 2;
                    textureDataSize = sizeof(Image::Handle) * numTextures;
                    tTexturePool.textures = (Image::Handle*) calloc(numTextures, sizeof(Image::Handle));
                    glGenBuffers(1, &uboTP);
                    glBindBuffer(GL_UNIFORM_BUFFER, uboTP);
                    glBufferData(GL_UNIFORM_BUFFER, textureDataSize, NULL, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_UNIFORM_BUFFER, 0);
                    glBindBufferRange(GL_UNIFORM_BUFFER, tpUniformIndex, uboTP, 0, textureDataSize);
                }
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
        TextureLayerData &layer = tDrawData.segments[unit / 4].layers[unit % 4];
        if (l->getSize()[0] == JGL_AUTOMATIC_SIZE) {
            layer.size[0] = l->getImageSize()[0];
            layer.controller[0] = 0u;
        }
        else {
            layer.size[0] = l->getSize()[0];
            layer.controller[0] = 1u;
        }

        if (l->getSize()[1] == JGL_AUTOMATIC_SIZE) {
            layer.size[1] = l->getImageSize()[1];
            layer.controller[1] = 0u;
        }
        else {
            layer.size[1] = l->getSize()[1];
            layer.controller[1] = 1u;
        }

        layer.position[0] = l->getPosition()[0];
        layer.position[1] = l->getPosition()[1];

        layer.imageSize[0] = l->getImageSize()[0];
        layer.imageSize[1] = l->getImageSize()[1];

        layer.factor[0] = l->getScalingFactor()[0];
        layer.factor[1] = l->getScalingFactor()[1];

        layer.channel = static_cast<uint32_t>(l->getSlot());

        layer.rotation = static_cast<float>(static_cast<long double>(l->getRotation()));

        layer.mode = static_cast<uint32_t>(l->getScalingMode());
    }

    void loadTextureUnit(Texture *l, size_t unit) {
        TextureLayerData &layer = tDrawData.segments[unit / 4].layers[unit % 4];
        layer.size[0] = l->getSize()[0];
        layer.controller[0] = 0u;
        layer.size[1] = l->getSize()[1];
        layer.controller[1] = 0u;
        layer.mode = 0u;

        layer.position[0] = 0.f;
        layer.position[1] = 0.f;

        layer.imageSize[0] = 0.f;
        layer.imageSize[1] = 0.f;

        layer.factor[0] = 1.f;
        layer.factor[1] = 1.f;

        layer.rotation = 0.f;
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
                        if (GLEW_ARB_bindless_texture) tTexturePool.textures[textureLoadCount * 2] = (*multiTexture)[i].getTexture()->handle();
                        else {
                            glActiveTexture(GL_TEXTURE0 + textureLoadCount);
                            glBindTexture(GL_TEXTURE_2D, (*multiTexture)[i].getTexture()->id());
                        }
                        loadTextureUnit(&(*multiTexture)[i], textureLoadCount);
                        ++textureLoadCount;
                    }
                    if ((i + 1) % 4 == 0) {
                        tDrawData.segments[activeSegment].link = multiTexture->getBlender(activeSegment, PHASE_0);
                        tDrawData.segments[activeSegment].connect = multiTexture->getBlender(activeSegment, PHASE_1);
                        tDrawData.segments[activeSegment].sep = textureLoadCount;
                        if (GLEW_ARB_bindless_texture && (*multiTexture).getMap(activeSegment).getTexture()) {
                            *((tTexturePool.textures + (CompositeTexture::availableSlots() * 2)) + activeSegment) = (*multiTexture).getMap(activeSegment).getTexture()->handle();
                        }
                        ++activeSegment;
                    }
                }

                fDrawData.textureCount = textureLoadCount;
        } else {
            if (singleTexture) {
                fDrawData.textureCount = 1;
                if (GLEW_ARB_bindless_texture) tTexturePool.textures[0] = singleTexture->handle();
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, singleTexture->id());
                }
                loadTextureUnit(singleTexture, 0);
            } else fDrawData.textureCount = 0;
        }

        if (fDrawData.textureCount) {
            UPDATE_UBO(uboT, tDrawData);
            if (GLEW_ARB_bindless_texture) {
                glBindBuffer(GL_UNIFORM_BUFFER, uboTP);
                uboData = glMapBufferRange(GL_UNIFORM_BUFFER, 0, textureDataSize, mapping);
                if (uboData) memcpy(uboData, tTexturePool.textures, textureDataSize);
                glUnmapBuffer(GL_UNIFORM_BUFFER);
            }
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

        vUniformIndex = glGetUniformBlockIndex(shader->id(), "JGLVertexDrawData");
        fUniformIndex = glGetUniformBlockIndex(shader->id(), "JGLFragmentDrawData");
        tUniformIndex = glGetUniformBlockIndex(shader->id(), "TextureDrawData");
        tpUniformIndex = glGetUniformBlockIndex(shader->id(), "TexturePool");
        if (fUniformIndex == GL_INVALID_INDEX || vUniformIndex == GL_INVALID_INDEX || tpUniformIndex == GL_INVALID_INDEX) {
            getCore()->errorHandler(0xa29, "Invalid shaders loaded! Could not locate JGL Uniform blocks.");
        } else {
            glUniformBlockBinding(shader->id(), vUniformIndex, 1);
            glUniformBlockBinding(shader->id(), fUniformIndex, 0);
            glUniformBlockBinding(shader->id(), tUniformIndex, 2);
            glUniformBlockBinding(shader->id(), tpUniformIndex, 3);
        }
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

    /*const jutil::Queue<const char*> UNIFORM_NAMES = {
        "material.ambient",     // 0
        "material.diffuse",     // 1
        "material.specular",    // 2
        "maetrial.shine",       // 3
        "mapTexture"            // 4
    };

    GLuint uboF, uboV, uboT, uboTP, uboS;
    unsigned fUniformIndex, vUniformIndex, sActiveP = 0, tpUniformIndex, sUniformIndex;
    GLvoid *uboData = NULL;
    Shader *sActive = NULL;
    jml::Vector4f cNorms;
    GLbitfield mapping = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
    bool initUniforms = false;
    //size_t textureDataSize;

    /*struct TextureLayerData {
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

    struct TextureSegment {
        TextureLayerData layers[4]; // 0 -> 256
        uint32_t sep;               // 256 -> 260
        uint32_t link;              // 260 -> 264
        uint32_t connect;           // 264 -> 268
        uint32_t __padding_0__;     // 268 -> 272
    }; | /

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

    /*struct Object::TextureSegmentData {
        TextureSegment segments[0xf]; // 0 -> 4352
    } sDrawData;

    struct TexturePool {
        Image::Handle *textures;
    } tTexturePool; | /

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
                //sDrawData = {0};
                fDrawData = {0};
                vDrawData = {0};
                INIT_UBO(uboV, vDrawData, vUniformIndex);
                INIT_UBO(uboF, fDrawData, fUniformIndex);
                //INIT_UBO(uboS, sDrawData, sUniformIndex);
                /*if (GLEW_ARB_bindless_texture) {
                    int numTextures = (CompositeTexture::availableSlots() + (CompositeTexture::availableSlots() / 4)) * 2;
                    textureDataSize = sizeof(Image::Handle) * numTextures;
                    tTexturePool.textures = (Image::Handle*) calloc(numTextures, sizeof(Image::Handle));
                    glGenBuffers(1, &uboTP);
                    glBindBuffer(GL_UNIFORM_BUFFER, uboTP);
                    glBufferData(GL_UNIFORM_BUFFER, textureDataSize, NULL, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_UNIFORM_BUFFER, 0);
                    glBindBufferRange(GL_UNIFORM_BUFFER, tpUniformIndex, uboTP, 0, textureDataSize);
                } | /
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

    /*void loadTextureUnit(TextureLayer *l, size_t unit) {
        TextureLayerData &layer = sDrawData.segments[unit / 4].layers[unit % 4];
        if (l->getSize()[0] == JGL_AUTOMATIC_SIZE) {
            layer.size[0] = l->getImageSize()[0];
            layer.controller[0] = 0u;
        }
        else {
            layer.size[0] = l->getSize()[0];
            layer.controller[0] = 1u;
        }

        if (l->getSize()[1] == JGL_AUTOMATIC_SIZE) {
            layer.size[1] = l->getImageSize()[1];
            layer.controller[1] = 0u;
        }
        else {
            layer.size[1] = l->getSize()[1];
            layer.controller[1] = 1u;
        }

        layer.position[0] = l->getPosition()[0];
        layer.position[1] = l->getPosition()[1];

        layer.imageSize[0] = l->getImageSize()[0];
        layer.imageSize[1] = l->getImageSize()[1];

        layer.factor[0] = l->getScalingFactor()[0];
        layer.factor[1] = l->getScalingFactor()[1];

        layer.channel = static_cast<uint32_t>(l->getSlot());

        layer.rotation = static_cast<float>(static_cast<long double>(l->getRotation()));

        layer.mode = static_cast<uint32_t>(l->getScalingMode());
    }

    void loadTextureUnit(Texture *l, size_t unit) {
        TextureLayerData &layer = sDrawData.segments[unit / 4].layers[unit % 4];
        layer.size[0] = l->getSize()[0];
        layer.controller[0] = 0u;
        layer.size[1] = l->getSize()[1];
        layer.controller[1] = 0u;
        layer.mode = 0u;

        layer.position[0] = 0.f;
        layer.position[1] = 0.f;

        layer.imageSize[0] = 0.f;
        layer.imageSize[1] = 0.f;

        layer.factor[0] = 1.f;
        layer.factor[1] = 1.f;

        layer.rotation = 0.f;
    } | /

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

        /*if (multiTexture) {
                size_t textureLoadCount = 0;
                size_t activeSegment = 0;
                for (size_t i = 0; i < CompositeTexture::availableSlots(); ++i) {
                    if ((*multiTexture)[i].getTexture() && (*multiTexture)[i].getTexture()->handle()) {
                        if (GLEW_ARB_bindless_texture) tTexturePool.textures[textureLoadCount * 2] = (*multiTexture)[i].getTexture()->handle();
                        else {
                            glActiveTexture(GL_TEXTURE0 + textureLoadCount);
                            glBindTexture(GL_TEXTURE_2D, (*multiTexture)[i].getTexture()->id());
                        }
                        loadTextureUnit(&(*multiTexture)[i], textureLoadCount);
                        ++textureLoadCount;
                    }
                    if ((i + 1) % 4 == 0) {
                        sDrawData.segments[activeSegment].link = multiTexture->getBlender(activeSegment, PHASE_0);
                        sDrawData.segments[activeSegment].connect = multiTexture->getBlender(activeSegment, PHASE_1);
                        sDrawData.segments[activeSegment].sep = textureLoadCount;
                        if (GLEW_ARB_bindless_texture && (*multiTexture).getMap(activeSegment).getTexture()) {
                            *((tTexturePool.textures + (CompositeTexture::availableSlots() * 2)) + activeSegment) = (*multiTexture).getMap(activeSegment).getTexture()->handle();
                        }
                        ++activeSegment;
                    }
                }

                fDrawData.textureCount = textureLoadCount;
        } else {
            if (singleTexture) {
                fDrawData.textureCount = 1;
                if (GLEW_ARB_bindless_texture) tTexturePool.textures[0] = singleTexture->handle();
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, singleTexture->id());
                }
                loadTextureUnit(singleTexture, 0);
            } else fDrawData.textureCount = 0;
        }

        if (fDrawData.textureCount) {
            UPDATE_UBO(uboS, sDrawData);
            if (GLEW_ARB_bindless_texture) {
                for (size_t i = 0; i < (textureDataSize) / (sizeof(Image::Handle) * 2); ++i) {
                    if (i < 24) {
                        jutil::out << "textures[";
                    } else {
                        jutil::out << "maps[";
                    }
                    jutil::out << i << "] = " << *(tTexturePool.textures + (i * 2)) << jutil::endl;
                }
                glBindBuffer(GL_UNIFORM_BUFFER, uboTP);
                uboData = glMapBufferRange(GL_UNIFORM_BUFFER, 0, textureDataSize, mapping);
                if (uboData) memcpy(uboData, tTexturePool.textures, textureDataSize);
                glUnmapBuffer(GL_UNIFORM_BUFFER);
            }
        } | /

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

        vUniformIndex = glGetUniformBlockIndex(shader->id(), "JGLVertexDrawData");
        fUniformIndex = glGetUniformBlockIndex(shader->id(), "JGLFragmentDrawData");
        //sUniformIndex = glGetUniformBlockIndex(shader->id(), "TextureSegmentData");
        //if (GLEW_ARB_bindless_texture) tpUniformIndex = glGetUniformBlockIndex(shader->id(), "TexturePool");
        if (fUniformIndex == GL_INVALID_INDEX || vUniformIndex == GL_INVALID_INDEX || sUniformIndex == GL_INVALID_INDEX) {
            getCore()->errorHandler(0xa29, "Invalid shaders loaded! Could not locate JGL Uniform blocks.");
        } else {
            glUniformBlockBinding(shader->id(), vUniformIndex, 0);
            glUniformBlockBinding(shader->id(), fUniformIndex, 1);
            //glUniformBlockBinding(shader->id(), sUniformIndex, 2);
            //if (GLEW_ARB_bindless_texture) glUniformBlockBinding(shader->id(), tpUniformIndex, 3);
        }
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
    }*/


}
