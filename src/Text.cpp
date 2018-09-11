#include "Text.h"

using namespace jml::literals;

namespace jgl {

    const char *VERTEX_TEXT = R"glsl(
        #version jgl vertex

        out vec4 vP;

        void main() {
            gl_Position = jglVertexShader();
            vP = jglGetVertexInput();
        }
    )glsl";

    const char *FRAGMENT_TEXT = R"glsl(
        #version jgl fragment
        #include jglVertexShader;

        uniform float al;

        ///resulting color of fragment
        layout(location = 0) out vec4 color;

        uniform uint sharpen;
        uniform sampler2D mask;

        float sKernal[9];
        vec2 sOffset[9];

        in vec4 vP;

        vec2 worldToTexture(vec2 vA) {
            return vec2((vA.x / 2.0f) + 0.5f, ((vA.y / 2.0f) + 0.5f) + (vA.y * -1.0f));
        }

        void main() {

            float step_w = 1.0/jglObjectSize.x;
            float step_h = 1.0/jglObjectSize.y;

            sOffset[0] = vec2(-step_w, -step_h);
            sOffset[1] = vec2(0.0, -step_h);
            sOffset[2] = vec2(step_w, -step_h);
            sOffset[3] = vec2(-step_w, 0.0);
            sOffset[4] = vec2(0.0, 0.0);
            sOffset[5] = vec2(step_w, 0.0);
            sOffset[6] = vec2(-step_w, step_h);
            sOffset[7] = vec2(0.0, step_h);
            sOffset[8] = vec2(step_w, step_h);

            sKernal[0] = 0.;
            sKernal[1] = -1.;
            sKernal[2] = 0.;
            sKernal[3] = -1.;
            sKernal[4] = 5.;
            sKernal[5] = -1.;
            sKernal[6] = 0.;
            sKernal[7] = -1.;
            sKernal[8] = 0.;

            vec4 jCol = jglFragmentShader();
            vec2 tC = worldToTexture(vec2(vP.x, vP.y));

            if (sharpen > 0u) {

                vec4 sum = vec4(0.0);

                for (int i = 0; i < 9; ++i) {
                    vec4 sColor = texture2D(mask, tC + sOffset[i]);
                    sum += sColor * sKernal[i];
                }

                color = vec4(jCol.r, jCol.g, jCol.b, jCol.a * sum.a);
            } else {
                color = vec4(jCol.r, jCol.g, jCol.b, jCol.a * texture2D(mask, tC).a);
            }
        }
    )glsl";

    Text::Text(const Font *f, const jutil::String &s, const Position &p, const Color &c) : Quad(p, 0, c), font(f), str(s), valid(false), line(0), filters(0) {
        useShader(Shader(VERTEX_TEXT, FRAGMENT_TEXT));
        glGenTextures(1, &textMask);
        glBindTexture(GL_TEXTURE_2D, textMask);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        buildTexture();
    }

    jutil::String Text::getString() const {
        return str;
    }
    const Font *Text::getFont() const {
        return font;
    }
    Text &Text::setFont(const Font *f) {
        font = f;
        valid = false;
        return *this;
    }
    Text &Text::setString(const jutil::String &s) {
        str = s;
        valid = false;
        return *this;
    }

    void Text::render(const Screen *scr) {
        if (!valid) buildTexture();
        shader.setUniform("mask", _JGL_TEXTURE_SEGMENT_LENGTH);
        //if (filters & Filter::SHARPEN) shader.setUniform("sharpen", (unsigned)true);
        glActiveTexture(_JGL_TEXT_SEGMENT);
        glBindTexture(GL_TEXTURE_2D, textMask);
        Quad::render(scr);
    }

    void Text::buildTexture() {
        maskSize = calcTextureSize();
        const Character *c = NULL;
        unsigned char *empty = (unsigned char*)calloc(maskSize.x() * maskSize.y(), sizeof(unsigned char));
        Dimensions cDim;
        glBindTexture(GL_TEXTURE_2D, textMask);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, maskSize.x(), maskSize.y(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, maskSize.x(), maskSize.y(), GL_ALPHA, GL_UNSIGNED_BYTE, empty);
        glBindTexture(GL_TEXTURE_2D, 0);
        unsigned cursor = 0;
        for (auto &i: str) {
            c = font->getCharacter(i);
            cDim = c->getSize();
            cursor += c->getBearing().x();
            auto offsetY = (line - cDim.y()) + (cDim.y() - c->getBearing().y());
            glCopyImageSubData(c->getTexture(), GL_TEXTURE_2D, 0, 0, 0, 0, textMask, GL_TEXTURE_2D, 0, cursor, offsetY, 0, cDim.x(), cDim.y(), 1);
            cursor += c->nextPosition().x();
        }
        size = maskSize;
        valid = true;
        free(empty);
    }

    Dimensions Text::calcTextureSize() {
        Dimensions r, cDim;
        const Character *c = NULL;
        for (auto &i: str) {
            c = font->getCharacter(i);
            cDim = c->getSize();
            r.x() += c->getBearing().x();
            auto h = cDim.y() + (cDim.y() - c->getBearing().y());
            if (h > r.y()) r.y() = h;
            if (cDim.y() > line) line = cDim.y();
            r.x() += c->nextPosition().x();
        }
        return r;
    }

    void Text::addFilter(uint8_t f) {
        filters |= f;
    }
    void Text::removeFilter(uint8_t f) {
        filters &= ~f;
    }
    bool Text::hasFilter(uint8_t f) const {
        return (filters & f);
    }
}


