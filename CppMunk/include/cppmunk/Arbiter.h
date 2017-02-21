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

        cpFloat getRestitution() const { return cpArbiterGetRestitution(arbiter); }
        void setRestitution(cpFloat value) { cpArbiterSetRestitution(arbiter, value); }

        cpFloat getFriction() const { return cpArbiterGetFriction(arbiter); }
        void setFriction(cpFloat value) { cpArbiterSetFriction(arbiter, value); }

        cpVect getSurfaceVelocity() const { return cpArbiterGetSurfaceVelocity(arbiter); }
        void setSurfaceVelocity(cpVect value) { cpArbiterSetSurfaceVelocity(arbiter, value); }

        cpDataPointer getUserData() const { return cpArbiterGetUserData(arbiter); }
        void setUserData(cpDataPointer value) { cpArbiterSetUserData(arbiter, value); }
        
    private:
        cpArbiter* arbiter;
    };
}

#endif /* CHIPMUNK_ARBITER_H */
