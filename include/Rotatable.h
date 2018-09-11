#ifndef JGL_ROTATABLE_H
#define JGL_ROTATABLE_H

#include <Angle.h>

namespace jgl {
    class Rotatable {
    public:
        Rotatable(const jml::Angle&);

        Rotatable &setRotation(const jml::Angle&);
        Rotatable &rotate(const jml::Angle&);

        const jml::Angle &getRotation() const;

        virtual ~Rotatable();
    protected:
        jml::Angle rotation;
    };
}

#endif // JGL_ROTATABLE_H
