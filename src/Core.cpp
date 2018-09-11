#include <IO/IO.h>
#include "jgl.h"

namespace jgl {
    Core::Core() : _window(getWindow()) {}
    void Core::eventHandler(Event e) {
        switch(e.type) {
        case Event::Type::CLOSE:
            end(0);

        default: break;
        }
    }

    void Core::errorHandler(uint16_t id, const char *msg) {
        jutil::err << id << ": " << msg << jutil::endl;
    }

    void Core::gameLoop() {
        while (open()) {
            _window->clear();



             pollEvents();


            if (!update()) {
                jgl::end(0);
                break;
            }


            _window->render();
            pause();

        }
    }
}
