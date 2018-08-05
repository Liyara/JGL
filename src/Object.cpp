#include "jgl.h"
#include "Matrix.h"
#include <Maths.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
        drawMode(GL_POLYGON) {
            uniforms.insert(shader.getUniformID("hastex"));
            uniforms.insert(shader.getUniformID("mode"));
            uniforms.insert(shader.getUniformID("wSize"));
            uniforms.insert(shader.getUniformID("rawPosition"));
            uniforms.insert(shader.getUniformID("rawSize"));
            uniforms.insert(shader.getUniformID("rotation"));
            uniforms.insert(shader.getUniformID("fcolor"));
            uniforms.insert(shader.getUniformID("lightCount"));
            uniforms.insert(shader.getUniformID("normal"));
            uniforms.insert(shader.getUniformID("cameraPos"));
            uniforms.insert(shader.getUniformID("material.ambient"));
            uniforms.insert(shader.getUniformID("material.diffuse"));
            uniforms.insert(shader.getUniformID("material.specular"));
            uniforms.insert(shader.getUniformID("maetrial.shine"));
        }

    Object::Object() : Object({0, 0}, {0, 0}, Color::White) {}

    void Object::draw() {

        auto colorNormal = color.normals();

        Shader::setActive(&shader);
        if (!formed) {
            formShape();
        }

        shader.setUniform(uniforms[0], (unsigned)hasTexture);

        shader.setUniform(uniforms[1], lighting());
        shader.setUniform(uniforms[2], static_cast<jml::Vector2f>(getWindowSize()));

        if (lighting() != JGL_LIGHTING_NONE) {
            jml::Vector3f nColor(colorNormal);
            auto lights = getLightsInScene();
            shader.setUniform(uniforms[7], (int)lights.size());
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

            shader.setUniform(uniforms[8], jml::Vector3u{0, 0, 1});
            shader.setUniform(uniforms[9], jml::Vector3d(getCameraPosition(), 1.0));

            shader.setUniform(uniforms[10], nColor);
            shader.setUniform(uniforms[11], nColor);
            shader.setUniform(uniforms[12], jml::Vector3f(material.specular.normals()));
            shader.setUniform(uniforms[13], material.shine);
        }



        shader.setUniform(uniforms[3], static_cast<jml::Vector2f>(position));
        shader.setUniform(uniforms[4], static_cast<jml::Vector2f>(size));
        shader.setUniform(uniforms[5], static_cast<float>(static_cast<long double>(rotation)));


        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindBuffer(GL_ARRAY_BUFFER, vao);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, components * sizeof(float), NULL);
        glPolygonMode(GL_FRONT, GL_FILL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (hasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, components * sizeof(float), (char*)0 + 3 * sizeof(float));
        }

        if (fill) {
            shader.setUniform(uniforms[6], colorNormal);
            glDrawArrays(drawMode, 0, vertexCount);
        }

        if (outline) {
            shader.setUniform(uniforms[6], outlineColor.normals());
            glLineWidth(outline);
            glPolygonMode(GL_FRONT, GL_LINE);
            glDrawArrays(GL_POLYGON, 0, vertexCount);
            glLineWidth(1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        Shader::setActive(NULL);
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

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(str, &width, &height, &nrChannels, 0);

        GLenum channels;

        if (nrChannels == 3) channels = GL_RGB;
        else if (nrChannels == 4) channels = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);

        if (data) {

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(data);

            hasTexture = true;
        } else {
            jutil::String err = jutil::String("Failed to load texture ") + jutil::String(str);
            char errC[err.size() + 1];
            err.array(errC);
            jgl::getCore()->errorHandler(0xf0,  errC);
        }

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
        vertexCount = polygon.size();

        unpackedPolygon.reserve(vertexCount * components);

        for (auto &i: polygon) {
            i[0] *= ASPECT;
            for (auto &j: i) {
                unpackedPolygon.insert(j);
            }
        }

        ///Enable use of object within OPENGL
        glGenBuffers(1, &vao);

        ///Bind object to the "GL_ARRAY_BUFFER" section of the OPENGL context
        glBindBuffer(GL_ARRAY_BUFFER, vao);

        ///allocate memory to the object we've just bound in "GLARRAY_BUFFER" in the GPU
        glBufferData(GL_ARRAY_BUFFER, vertexCount * components * sizeof(float), &(unpackedPolygon[0]), GL_STATIC_DRAW);

        ///unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        formed = true;

        return *this;
    }

    Object &Object::rotate(const jml::Angle &r) {
        rotation += r;
        return *this;
    }

    jutil::Queue<jml::Vertex> Object::getVertices() {

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
