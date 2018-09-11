#include "Font.h"
#include "jgl.h"

#ifdef JUTIL_WINDOWS
    #define WINVER 0x0600
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT
    #endif
    #define _WIN32_WINNT 0x0600
    #include <windows.h>
    #include <Shlobj.h>
    #include <initguid.h>
    #include <Knownfolders.h>
    #include <Objbase.h>
#endif // JUTIL_WINDOWS

namespace jgl {
    Font::Font(jutil::String f, unsigned s) : file(f), size(s) {


        if (!file.find('\\') && !file.find('/')) {

            if (!file.find('.')) file += ".ttf";

            #ifdef JUTIL_WINDOWS
                wchar_t *path = NULL;
                char cPath[255];
                SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &path);
                wcstombs(cPath, path, 255);
                file = jutil::String(cPath) + '\\' + file;
            #elif defined JUTIL_LINUX
                file = jutil::String("/usr/share/fonts/") + file;
            #endif // JUTIL_WINDOWS
        }

        if (FT_Init_FreeType(&loader))
            if (getCore()) getCore()->errorHandler(0x43, "Font failed to initialize!");

        for (char i = 0x20; i < 0x80 && i > 0x1f; ++i) characters.insert(i, new Character(loader, file, i, s));
    }

    jutil::String Font::getFile() const {
        return file;
    }
    unsigned Font::getSize() const {
        return size;
    }
    const Character *Font::getCharacter(char c) const {
        return characters[c];
    }
    Font::~Font() {
        FT_Done_FreeType(loader);
    }
}
