#ifndef ELLIPSE_H
#define	ELLIPSE_H

#include "Poly.h"

namespace jgl {

	class Ellipse : public Object {
	public:

		Ellipse(const Position&, const Dimensions&, const Color& = Color::White);

        long double area() const;

        jml::Vector2ld getRadii() const;

    private:
        jutil::Queue<jutil::Queue<long double> > genVAO();

    };

}

#endif //CIRCLE_H
