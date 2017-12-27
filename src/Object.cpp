#include "jgl.h"
#include "Matrix.h"
#include <Maths.h>

namespace jgl {
    Object::Object(Position p, Dimensions d, Color c) : position(p), size(d), color(c), hasTexture(false), rotation(0), formed(false), components(0), material() {}

    void Object::draw() {

        if (!formed) {
            jgl::getCore()->errorHandler(10, "Attempted drawing of ill-formed object.");
        }

        glUniform1ui(getUniform("hastex"), (unsigned)hasTexture);
        glUniform1i(getUniform("mode"), lighting());
        glUniform2f(getUniform("wsize"), getWindowSize().x(), getWindowSize().y());
        if (lighting()) {
            jutil::Queue<LightSource> lights = getLightsInScene();
            glUniform1i(getUniform("lightCount"), lights.size());
            for (unsigned i = 0; i < lights.size(); ++i) {
                jutil::String baseStr = "lights[" + jutil::String(i),
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

                glUniform2f(getUniform(pos_s), lights[i].position.x() / 2.f, -lights[i].position.y() / 2.f);
                glUniform1f(getUniform(int_s), lights[i].intensity);
                glUniform3f(getUniform(color_s), (float)lights[i].color.red() / 255.f,
                            (float)lights[i].color.green() / 255.f, (float)lights[i].color.blue() / 255.f);

            }

            glUniform3f(getUniform("normal"), 0, 0, 1);
            glUniform3f(getUniform("cameraPos"), getCameraPosition().x(), getCameraPosition().y(), 1.f);

            glUniform3f(getUniform("material.ambient"), color.red() / 255.f,color.green() / 255.f, color.blue() / 255.f);
            glUniform3f(getUniform("material.diffuse"), color.red() / 255.f, color.green() / 255.f, color.blue() / 255.f);
            glUniform3f(getUniform("material.specular"), material.specular.red() / 255.f, material.specular.green() / 255.f, material.specular.blue() / 255.f);
            glUniform1f(getUniform("material.shine"), material.shine);
        }

        float mvpArr[16];
        mvp.array(mvpArr);

        glUniform2f(getUniform("offset"), ((position.x() / getWindowSize().x())), (position.y() / getWindowSize().y()));
        glUniform2f(getUniform("rawPosition"), position.x(), position.y());
        glUniformMatrix4fv(getUniform("mvp"), 1, GL_TRUE, mvpArr);
        glUniform1f(getUniform("yRatio"), (float)getWindowSize().y() / ((float)getWindowSize().x() / 2.0f));
        glUniform4f(getUniform("fcolor"),
            Color::normal(color.red()),
            Color::normal(color.green()),
            Color::normal(color.blue()),
            Color::normal(color.alpha())
        );

        glUniform1i(getUniform("tex"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindBuffer(GL_ARRAY_BUFFER, vao);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), NULL);

        if (hasTexture) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, components * sizeof(float), (char*)0 + 3 * sizeof(float));
        }

        glDrawArrays(GL_POLYGON, 0, vertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Object &Object::setRotation(const jml::Angle &angle) {
        rotation = angle;
        return generateMVP();
    }

    Object &Object::setMaterial(const Material &m) {
        material = m;
        return *this;
    }

    Object &Object::setColor(const Color &c) {
        color = c;
        return *this;
    }
    Object::Object() {}
    Color Object::getColor() const {
        return color;
    }
    Object &Object::setPosition(const Position &p) {
        position = p;
        return generateMVP();
    }
    Position Object::getPosition() const {
        return position;
    }
    Object &Object::setSize(const Dimensions &d) {
        size = d;
        return generateMVP();
    }
    Dimensions Object::getSize() const {
        return size;
    }
    Object &Object::move(Position offset) {
        position[0] += offset[0];
        position[1] += offset[1];
        return generateMVP();
    }
    Object &Object::scale(jml::Vector2d s) {
        size.x() *= s.x();
        size.y() *= s.y();
        return generateMVP();
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

        return generateMVP();
    }

    Object &Object::rotate(const jml::Angle &r) {
        rotation += r;
        return generateMVP();
    }

    Object &Object::generateMVP() {
        if (formed) {
            GLint m_viewport[4];
            glGetIntegerv(GL_VIEWPORT, m_viewport);

            jml::Vector2f s = {(float)size.x() / (float)m_viewport[2], (float)size.y() / (float)m_viewport[3]};
            const float ASPECT = ((float)m_viewport[2] / (float)m_viewport[3]);

            jml::Transformation model = jml::identity<long double, 4>();
            model = jml::scale({s.x(), s.y(), 1.0L}, model);
            model = jml::rotate(rotation, {0, 0, 1}, model);
            jml::Transformation projection = jml::ortho(-ASPECT, ASPECT, -1.0L, 1.0L, -1.0L, 1.0L);

            mvp = projection * model;

        }

        return *this;
    }
}
