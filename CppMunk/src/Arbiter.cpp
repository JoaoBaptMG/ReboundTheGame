#include "Arbiter.h"

namespace Chipmunk
{
    Arbiter::Arbiter(cpArbiter* a) : arbiter(a)
    { }
    
    cpBody* Arbiter::getBodyA()
    {
        cpBody* a;
        cpBody* b;
        cpArbiterGetBodies(arbiter, &a, &b);
        return a;
    }
    
    cpBody* Arbiter::getBodyB()
    {
        cpBody* a;
        cpBody* b;
        cpArbiterGetBodies(arbiter, &a, &b);
        return b;
    }

    cpShape* Arbiter::getShapeA()
    {
        cpShape* a;
        cpShape* b;
        cpArbiterGetShapes(arbiter, &a, &b);
        return a;
    }
    
    cpShape* Arbiter::getShapeB()
    {
        cpShape* a;
        cpShape* b;
        cpArbiterGetShapes(arbiter, &a, &b);
        return b;
    }
}
