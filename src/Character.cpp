#include "Character.h"
#include "jgl.h"

namespace jgl {
    Character::Character(char c, const Position &p, const Font &f, const Color &col) : raw(c), font(f) {

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        position = p;
        color = col;

        char fileArr[font.file.size() + 1];
        font.file.array(fileArr);

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            jgl::getCore()->errorHandler(50, "FreeType failed to initialize!");

        FT_Face face;
        if (FT_New_Face(ft, fileArr, 0, &face)) {
            jutil::String ftfcerr = "Failed to load face " + font.file + '!';
            char ftfcerrcstr[ftfcerr.size() + 1];
            ftfcerr.array(ftfcerrcstr);
            jgl::getCore()->errorHandler(51, ftfcerrcstr);
        }


        FT_Set_Pixel_Sizes(face, 0, font.size);
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) jutil::out << "Error load char" << jutil::endl;

        auto dat = face->glyph;

        size.x() = dat->bitmap.width;
        size.y() = dat->bitmap.rows;

        bearing.x() = dat->bitmap_left;
        bearing.y() = dat->bitmap_top;

        long double nx = (dat->advance.x >> 6);
        long double ny = (dat->advance.y >> 6);

        next = Position({nx, ny});

        origin.y() = (position.y() + (size.y() / 2.0)) - (size.y() - bearing.y());
        origin.x() = (position.x() - (size.x() / 2.0)) - bearing.x();

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            size.x(),
            size.y(),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            dat->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        this->formShape();
        hasTexture = true;

    }

    Font Character::getFont() const {
        return font;
    }

    Character::~Character() {}

    char Character::decode() const {
        return raw;
    }

    Position Character::getOrigin() const {
        return origin;
    }

    Position Character::getBearing() const {
        return bearing;
    }

    void Character::draw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Shader::setActive(&(this->shader));
        this->shader.setUniform("isText", (unsigned)true);
        Shader::setActive(NULL);
        Quad::draw();
        Shader::setActive(&(this->shader));
        this->shader.setUniform("isText", (unsigned)false);
        Shader::setActive(NULL);
        glDisable(GL_BLEND);
        glBlendFunc(0, 0);
    }

    Position Character::nextPosition() const {
        return next;
    }

}
