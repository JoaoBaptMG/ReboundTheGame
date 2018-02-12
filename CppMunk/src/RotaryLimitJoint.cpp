#include "RotaryLimitJoint.h"
#include "Body.h"

namespace cp
{
    RotaryLimitJoint::RotaryLimitJoint(std::shared_ptr<Body> bodyA,
                                       std::shared_ptr<Body> bodyB,
                                       cpFloat min,
                                       cpFloat max) :
    Constraint(cpRotaryLimitJointNew(*bodyA, *bodyB, min, max),
               bodyA, bodyB)
    {}
}
