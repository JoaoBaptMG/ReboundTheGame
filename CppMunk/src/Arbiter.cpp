#include "Arbiter.h"

namespace Chipmunk
{
    Arbiter::Arbiter(cpArbiter* a) : arbiter(a)
    { }
    
    Body Arbiter::getBodyA()
    {
        cpBody* a;
        cpBody* b;
        cpArbiterGetBodies(arbiter, &a, &b);
        return Body(a);
    }
    
    Body Arbiter::getBodyB()
    {
        cpBody* a;
        cpBody* b;
        cpArbiterGetBodies(arbiter, &a, &b);
        return Body(b);
    }
}
