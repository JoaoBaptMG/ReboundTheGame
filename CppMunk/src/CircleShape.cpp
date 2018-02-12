#include "CircleShape.h"
#include "Body.h"

namespace cp
{
    CircleShape::CircleShape(std::shared_ptr<Body> body,
                             cpFloat radius,
                             cpVect offset) :
    Shape(cpCircleShapeNew(body ? (*body) :
                                    (cpBody*)0, radius, offset), body)
    { }
}
