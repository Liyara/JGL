#ifndef CORE_H
#define CORE_H

#include "Event.h"
#include <Core/integers.h>
#include "Window.h"

namespace jgl {
    class Core {
    public:
        Core();
        virtual bool update() = 0;
        virtual void eventHandler(Event);
        virtual void errorHandler(uint16_t, const char*);
        virtual void infoHandler(const char*);
        virtual void gameLoop();
    protected:
        Window *const _window;
    };
}

#endif // CORE_H
