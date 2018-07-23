#include "jgl.h"
#include "Matrix.h"
#include <Maths.h>

namespace jgl {
    Object::Object(const Position &p, const Dimensions &d, const Color &c) :
        position(p),
        size(d),
        color(c),
        hasTexture(false),
        rotation(0),
        formed(false),
        components(0),
        material(),
        outline(0),
        fill(true),
        outlineColor(c),
        shader(getDefaultShader()),
        drawMode(GL_POLYGON) {}

    Object::Object() : Object({0, 0}, {0, 0}, Color::White) {}

    void Object::draw() {

        auto colorNormal = color.normals();

        Shader::setActive(shader);
        if (!formed) {
            jgl::getCore()->errorHandler(10, "Attempted drawing of ill-formed object.");
        }
        shader.setUniform("hastex", (unsigned)hasTexture);
        shader.setUniform("mode", lighting());
        shader.setUniform("wsize", getWindowSize());

        if (lighting()) {
            jml::Vector3f nColor(colorNormal);
            auto lights = getLightsInScene();
            shader.setUniform("lightCount", (int)lights.size());
            for (size_t i = 0; i < lights.size(); ++i) {

                auto &light = lights[i];
                jml::Vector3f lColor(light.color.normals());

                jutil::String
                    baseStr = "lights[" + jutil::String(i),
                    posStr = "].pos",
                    intStr = "].intensity",
                    colorStr = "].c"
                ;

                char
                    pos_s[baseStr.size() + posStr.size() + 1],
                    int_s[baseStr.size() + intStr.size() + 1],
                    color_s[baseStr.size() + colorStr.size() + 1]
                ;

                (baseStr + posStr).array(pos_s);
                (baseStr + intStr).array(int_s);
                (baseStr + colorStr).array(color_s);

                jml::Vector2f pos_v {static_cast<float>(light.position.x() + getWindowSize().x() / 2.0), static_cast<float>(-light.position.y() + getWindowSize().y() / 2.0)};

                shader.setUniform(pos_s, pos_v);

                shader.setUniform(int_s, light.intensity);
                shader.setUniform(color_s, lColor);

            }

            shader.setUniform("normal", jml::Vector3u{0, 0, 1});
            shader.setUniform("cameraPos", jml::Vector3d(getCameraPosition(), 1.0));

            shader.setUniform("material.ambient", nColor);
            shader.setUniform("material.diffuse", nColor);
            shader.setUniform("material.specular", jml::Vector3f(material.specular.normals()));
            shader.setUniform("material.shine", material.shine);
        }

        shader.setUniform("offset", jml::Vector2f{static_cast<float>(position.x() / getWindowSize().x()), static_cast<float>(position.y() / getWindowSize().y())});
        shader.setUniform("rawPosition", position);

        shader.setUniform("yRatio", (float)getWindowSize().y() / ((float)getWindowSize().x() / 2.0f));
        shader.setUniform("tex", 0);

        generateMVP();
        shader.setUniform("mvp", mvp);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindBuffer(GL_ARRAY_BUFFER, vao);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), NULL);
        glPolygonMode(GL_FRONT, GL_FILL);

        if (hasTexture) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, components * sizeof(float), (char*)0 + 3 * sizeof(float));
        }

        if (fill) {
            shader.setUniform("fcolor", colorNormal);
            glDrawArrays(drawMode, 0, vertexCount);
        }

        if (outline) {
            shader.setUniform("fcolor", outlineColor.normals());
            glLineWidth(outline);
            glPolygonMode(GL_FRONT, GL_LINE);
            glDrawArrays(GL_POLYGON, 0, vertexCount);
            glLineWidth(1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        Shader::setActive(0);
    }

    Object &Object::setRotation(const jml::Angle &angle) {
        rotation = angle;
        return *this;
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
    Object &Object::setPosition(const Position &p) {
        position = p;
        return *this;
    }
    Position Object::getPosition() const {
        return position;
    }
    Object &Object::setSize(const Dimensions &d) {
        size = d;
        return *this;
    }
    Dimensions Object::getSize() const {
        return size;
    }
    Object &Object::move(Position offset) {
        position[0] += offset[0];
        position[1] += offset[1];
        return *this;
    }
    Object &Object::scale(jml::Vector2d s) {
        size.x() *= s.x();
        size.y() *= s.y();
        return *this;
    }
    Object &Object::setTexture(const char *str) {
        ilGenImages(1, &image);
        ilBindImage(image);
        if (ilLoadImage(str)) {
            void *data = ilGetData();
            int
                iw = ilGetInteger(IL_IMAGE_WIDTH),
                ih = ilGetInteger(IL_IMAGE_HEIGHT),
                it = ilGetInteger(IL_IMAGE_TYPE),
                id = ilGetInteger(IL_IMAGE_FORMAT)
            ;

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(GL_TEXTURE_2D, 0, id, iw, ih, 0, id, it, data);

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        } else {

        }
        ilBindImage(0);
        hasTexture = true;
        return *this;
    }

    Object &Object::setTexture(GLuint tex) {
        texture = tex;
        hasTexture = texture;
        return *this;
    }
    GLuint *Object::getTexture() const {
        if (hasTexture) {
            return (GLuint*)&texture;
        } else {
            return 0;
        }
    }
    Object::~Object() {

    }

    Object &Object::formShape() {
        GLint m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);
        const float ASPECT = ((float)m_viewport[2] / (float)m_viewport[3]);
        polygon = genVAO();

        components = polygon[0].size();

        jutil::Queue<float> unpackedPolygon;

        vertexCount = polygon.size();

        for (auto &i: polygon) {
            i[0] *= ASPECT;
            for (auto &j: i) {
                unpackedPolygon.insert(j);
            }
        }
        size_t num = sizeof(float) * unpackedPolygon.size();
        float *vp = (float*)malloc(num);
        for (uint16_t i = 0; i < unpackedPolygon.size(); ++i) {
            vp[i] = unpackedPolygon[i];
        }

        ///Enable use of object within OPENGL
        glGenBuffers(1, &vao);

        ///Bind object to the "GL_ARRAY_BUFFER" section of the OPENGL context
        glBindBuffer(GL_ARRAY_BUFFER, vao);

        ///allocate memory to the object we've just bound in "GLARRAY_BUFFER" in the GPU
        glBufferData(GL_ARRAY_BUFFER, vertexCount * components * sizeof(float), vp, GL_STATIC_DRAW);

        ///unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        formed = true;

        return *this;
    }

    Object &Object::rotate(const jml::Angle &r) {
        rotation += r;
        return *this;
    }

    Object &Object::generateMVP() {
        if (formed) {
            GLint m_viewport[4];
            glGetIntegerv(GL_VIEWPORT, m_viewport);

            jml::Vector2f s = {(float)size.x() / (float)m_viewport[2], (float)size.y() / (float)m_viewport[3]};
            const float ASPECT = ((float)m_viewport[2] / (float)m_viewport[3]);

            jml::Transformation model = jml::identity<long double, 4>();
            model = jml::scale({s, 1.0L}, model);
            model = jml::rotate(rotation, {0, 0, 1}, model);
            jml::Transformation projection = jml::ortho(-ASPECT, ASPECT, -1.0L, 1.0L, -1.0L, 1.0L);

            mvp = projection * model;

        }

        return *this;
    }

    jutil::Queue<jutil::Queue<long double> > Object::getVAO() const {
        return polygon;
    }

    jml::Angle Object::getRotation() const {
        return rotation;
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
        return *this;
    }
    Object &Object::setMode(GLenum m) {
        drawMode = m;
        return *this;
    }
    GLenum Object::getMode() const {
        return drawMode;
    }
}
