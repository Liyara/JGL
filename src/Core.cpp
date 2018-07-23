#include <IO/IO.h>
#include "jgl.h"

namespace jgl {
    Core::Core() {}
    void Core::eventHandler(Event e) {
        switch(e.type) {
            case Event::KEY: {
                switch(e.code) {
                    case Event::ESCAPE: {
                        break;
                    }
                    default: break;
                }
                break;
            }
            default: break;
        }
    }

    void Core::errorHandler(uint16_t id, const char *msg) {
        jutil::err << id << ": " << msg << jutil::endl;
    }

    void Core::gameLoop() {
        while (open()) {
            clear();
            pollEvents();

            if (!update()) {
                jgl::end(0);
                break;
            }

            display();
            pause();
        }
    }
}
