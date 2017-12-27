#ifndef CIRCLE_H
#define	CIRCLE_H

#include "Poly.h"

namespace jgl {

	class Circle : public Object {
	public:

		Circle(Position, unsigned, Color = Color::White);

	float area();

    private:
        Circle();
        unsigned radius;
        jutil::Queue<jutil::Queue<float> > genVAO();

    };

}

#endif //CIRCLE_H
