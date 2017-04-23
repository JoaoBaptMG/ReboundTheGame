#include "GrooveJoint.h"
#include "Body.h"

namespace cp
{
    GrooveJoint::GrooveJoint(std::shared_ptr<Body> bodyA,
                             std::shared_ptr<Body> bodyB,
                             cpVect groove_a,
                             cpVect groove_b,
                             cpVect anchorB) :
    Constraint(cpGrooveJointNew(*bodyA, *bodyB, groove_a, groove_b, anchorB),
               bodyA, bodyB)
    {}
}
