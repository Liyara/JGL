#include "Character.h"
#include "jgl.h"

namespace jgl {
    Character::Character(FT_Library ft, jutil::String f, char c, unsigned s) : raw(c), file(f), fSize(s) {

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        char fileArr[file.size() + 1];
        file.array(fileArr);

        FT_Face face;

        auto ftErr = FT_New_Face(ft, fileArr, 0, &face);

        if (ftErr) {
            jutil::String ftfcerr = "Failed to load face " + file + '!';
            char ftfcerrcstr[ftfcerr.size() + 1];
            ftfcerr.array(ftfcerrcstr);
            if (getCore()) jgl::getCore()->errorHandler(51, ftfcerrcstr);
        }


        FT_Set_Pixel_Sizes(face, 0, fSize);

        ftErr = FT_Load_Char(face, c, FT_LOAD_RENDER);

        if (ftErr) jutil::out << ftErr << ": Error load char" << jutil::endl;
        else {

            auto dat = face->glyph;

            size.x() = dat->bitmap.width;
            size.y() = dat->bitmap.rows;

            bearing.x() = dat->bitmap_left;
            bearing.y() = dat->bitmap_top;

            float nx = (dat->advance.x >> 6);
            float ny = (dat->advance.y >> 6);

            next = Position({nx, ny});

            origin.y() = ((size.y() / 2.0)) - (size.y() - bearing.y());
            origin.x() = (-(size.x() / 2.0)) - bearing.x();

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_ALPHA,
                size.x(),
                size.y(),
                0,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                dat->bitmap.buffer
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);

            FT_Done_Face(face);
        }


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

    Position Character::nextPosition() const {
        return next;
    }

    unsigned Character::getFontSize() const {
        return fSize;
    }
    jutil::String Character::getFontFile() const {
        return file;
    }

    uint32_t Character::getTexture() const {
        return texture;
    }

    Dimensions Character::getSize() const {
        return size;
    }

}
