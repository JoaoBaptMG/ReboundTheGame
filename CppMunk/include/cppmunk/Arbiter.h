#ifndef CHIPMUNK_ARBITER_H
#define CHIPMUNK_ARBITER_H

#include "Body.h"
#include <chipmunk.h>

namespace Chipmunk
{
    class Arbiter
    {
    public:
        Arbiter(cpArbiter*);
        Body getBodyA();
        Body getBodyB();
    private:
        cpArbiter* arbiter;
    };
}

#endif /* CHIPMUNK_ARBITER_H */
