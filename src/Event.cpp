#include "Event.h"

namespace jgl {
    Event::Event(Type t, double x, double y) {
        type = t;
        this->x = x;
        this->y = y;
    }

    Event::Event(Type t, int c, Action a, int m) {
        type = t;
        code = c;
        action = a;
        mods = m;
    }
}
