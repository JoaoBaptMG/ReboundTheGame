#include "Body.h"
#include "Shape.h"
#include "Arbiter.h"

namespace cp
{
    const Body::StaticTag Body::Static {};
    const Body::KinematicTag Body::Kinematic {};
    
    Body::Body(cpFloat mass, cpFloat inertia) :
    _body(cpBodyNew(mass, inertia))
    { }
    
    Body::Body(Body&& other) :
    _body(other._body)
    {
        other._body = nullptr;
    }

    Body::Body(StaticTag) :
    _body(cpBodyNewStatic())
    { }

    Body::Body(KinematicTag) :
    _body(cpBodyNewKinematic())
    {
        
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

    void Body::eachArbiter(std::function<void(Arbiter)> func)
    {
        cpBodyEachArbiter(_body, [](cpBody* body, cpArbiter* arbiter, void* data)
        {
            (*(std::function<void(Arbiter)>*)data)(Arbiter(arbiter));
        }, (void*)&func);
    }
}
