#include "Event.h"

namespace jgl {
    Event::Event(Type t, double xx, double yy) : type(t), x(xx), y(yy) {}

    Event::Event(Type t, int c, Action a, int m) : type(t), code(c), action(a), mods(m) {}

    Event::Event(Type t, int w, int h) : type(t), width(w), height(h) {}

    Event::Event(Type t) : type(t) {}
}
