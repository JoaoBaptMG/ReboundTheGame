#include "Body.h"
#include "Shape.h"

namespace Chipmunk
{
    Body::Body(cpFloat mass, cpFloat inertia) :
    _body(cpBodyNew(mass, inertia))
    { }
    
    Body::Body(Body&& other) :
    _body(other._body)
    {
        other._body = nullptr;
    }
    
    Body::Body(cpBody* body) :
    _body(body)
    { }
    
    Body::operator cpBody*() const
    {
        return _body;
    }
    
    Body::~Body()
    {
        if (_body != NULL)
            cpBodyFree(_body);
    }
    
    void Body::activateStatic(std::shared_ptr<Shape> filter) { cpBodyActivateStatic(_body, *filter); };
}
