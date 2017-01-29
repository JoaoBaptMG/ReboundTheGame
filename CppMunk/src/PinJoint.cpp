#include "PinJoint.h"
#include "Body.h"

namespace Chipmunk
{
    PinJoint::PinJoint(std::shared_ptr<Body> bodyA,
                       std::shared_ptr<Body> bodyB,
                       cpVect anchorA,
                       cpVect anchorB) :
    Constraint(cpPinJointNew(*bodyA, *bodyB, anchorA, anchorB),
               bodyA, bodyB)
    {}
}