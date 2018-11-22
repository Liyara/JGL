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

    ShaderFile *VERTEX_FILE;
    ShaderFile *FRAGMENT_FILE;
    Shader *textShader;

    void textInit() {
        VERTEX_FILE = new ShaderFile(VERTEX, VERTEX_TEXT);
        FRAGMENT_FILE = new ShaderFile(FRAGMENT, FRAGMENT_TEXT);
        textShader = new Shader({VERTEX_FILE, FRAGMENT_FILE});
    }

    void textDestroy() {
        delete VERTEX_FILE;
        delete FRAGMENT_FILE;
        delete textShader;
    }

    Text::Text(const Font *f, const jutil::String &s, const Position &p, const Color &c) : Quad(p, 0, c), font(f), str(s), valid(false), line(0), filters(0) {
        useShader(textShader);
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
        shader->setUniform("mask", _JGL_TEXTURE_SEGMENT_LENGTH);
        glActiveTexture(_JGL_TEXT_SEGMENT);
        glBindTexture(GL_TEXTURE_2D, textImage.id());
        Quad::render(scr);
    }

    void Text::buildTexture() {
        maskSize = calcTextureSize();
        const Character *c = NULL;
        unsigned char *empty = (unsigned char*)calloc(maskSize.x() * maskSize.y(), sizeof(unsigned char));
        Dimensions cDim;
        textImage.setSize(maskSize);
        textImage->setSize(maskSize);
        textImage->clear(0);

        unsigned cursor = 0;
        for (auto &i: str) {
            c = font->getCharacter(i);
            cDim = c->getSize();
            cursor += c->getBearing().x();
            auto offsetY = (line - cDim.y()) + (cDim.y() - c->getBearing().y());
            c->getImage().blit(*textImage, 0, {cursor, offsetY}, cDim);
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


