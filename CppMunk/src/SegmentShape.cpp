#include "SegmentShape.h"
#include "Body.h"

namespace Chipmunk
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
    
    cpVect SegmentShape::getA(const std::shared_ptr<Shape> shape)
    {
        return cpSegmentShapeGetA(*shape);
    }
    
    cpVect SegmentShape::getB(const std::shared_ptr<Shape> shape)
    {
        return cpSegmentShapeGetB(*shape);
    }
    
    cpVect SegmentShape::getNormal(const std::shared_ptr<Shape> shape)
    {
        return cpSegmentShapeGetNormal(*shape);
    }
    
    cpFloat SegmentShape::getRadius(const std::shared_ptr<Shape> shape)
    {
        return cpSegmentShapeGetRadius(*shape);
    }
}