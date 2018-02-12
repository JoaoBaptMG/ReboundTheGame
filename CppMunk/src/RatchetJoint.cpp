#include "RatchetJoint.h"
#include "Body.h"

namespace cp
{
    RatchetJoint::RatchetJoint(std::shared_ptr<Body> bodyA,
                               std::shared_ptr<Body> bodyB,
                               cpFloat phase,
                               cpFloat ratchet) :
    Constraint(cpRatchetJointNew(*bodyA, *bodyB, phase, ratchet),
    bodyA, bodyB)
    {}
}
