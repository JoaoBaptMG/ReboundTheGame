#include "SegmentShape.h"
#include "Body.h"

namespace cp
{
    SegmentShape::SegmentShape(std::shared_ptr<Body> body,
                               cpVect a, cpVect b,
                               cpFloat radius) :
    Shape(cpSegmentShapeNew(body ? (*body) :
                                    (cpBody*)NULL, a, b, radius), body)
    { }
    
    
    void SegmentShape::setNeighbors(std::shared_ptr<Shape> shape,
                                    cpVect prev,
                                    cpVect next)
    {
        cpSegmentShapeSetNeighbors(*shape, prev, next);
    }
    
    cpVect SegmentShape::getA()
    {
        return cpSegmentShapeGetA(_shape);
    }
    
    cpVect SegmentShape::getB()
    {
        return cpSegmentShapeGetB(_shape);
    }
    
    cpVect SegmentShape::getNormal()
    {
        return cpSegmentShapeGetNormal(_shape);
    }
    
    cpFloat SegmentShape::getRadius()
    {
        return cpSegmentShapeGetRadius(_shape);
    }
}
