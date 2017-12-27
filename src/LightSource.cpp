#include "LightSource.h"

namespace jgl {
    LightSource::LightSource(jml::Vector2f p, float i, Color c) {
        position[0] = p[0];
        position[1] = p[1];
        intensity = i;
        color = c;
    }
    LightSource::LightSource() {
        position = {0, 0};
        intensity = 0;
        color = Color::White;
    }
    bool LightSource::operator==(const LightSource &l) const {
        return (position == l.position && intensity == l.intensity && color == l.color);
    }
    bool LightSource::operator!=(const LightSource &l) const {
        return !((*this) == l);
    }
}
