#ifndef CORE_H
#define CORE_H

#include "Event.h"
#include <Core/integers.h>

namespace jgl {
    class Core {
    public:
        Core();
        virtual bool loop() = 0;
        virtual void eventHandler(Event);
        virtual void errorHandler(uint16_t, const char*);
    };
}

#endif // CORE_H
