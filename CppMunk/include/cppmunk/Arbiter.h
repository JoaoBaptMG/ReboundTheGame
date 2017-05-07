#ifndef CHIPMUNK_ARBITER_H
#define CHIPMUNK_ARBITER_H

#include "Body.h"
#include <chipmunk.h>

namespace cp
{
    class Arbiter
    {
    public:
        Arbiter(cpArbiter*);
        cpBody* getBodyA();
        cpBody* getBodyB();
        cpShape* getShapeA();
        cpShape* getShapeB();

        cpFloat getRestitution() const { return cpArbiterGetRestitution(arbiter); }
        void setRestitution(cpFloat value) { cpArbiterSetRestitution(arbiter, value); }

        cpFloat getFriction() const { return cpArbiterGetFriction(arbiter); }
        void setFriction(cpFloat value) { cpArbiterSetFriction(arbiter, value); }

        cpVect getSurfaceVelocity() const { return cpArbiterGetSurfaceVelocity(arbiter); }
        void setSurfaceVelocity(cpVect value) { cpArbiterSetSurfaceVelocity(arbiter, value); }

        cpDataPointer getUserData() const { return cpArbiterGetUserData(arbiter); }
        void setUserData(cpDataPointer value) { cpArbiterSetUserData(arbiter, value); }

        cpVect totalImpulse() const { return cpArbiterTotalImpulse(arbiter); }
        int getCount() const { return cpArbiterGetCount(arbiter); }
        cpVect getNormal() const { return cpArbiterGetNormal(arbiter); }
        cpVect getPointA(int i) const { return cpArbiterGetPointA(arbiter, i); }
        cpVect getPointB(int i) const { return cpArbiterGetPointB(arbiter, i); }
        cpFloat getDepth(int i) const { return cpArbiterGetDepth(arbiter, i); }

        operator cpArbiter*() const { return arbiter; }
        
    private:
        cpArbiter* arbiter;
    };
}

#endif /* CHIPMUNK_ARBITER_H */
