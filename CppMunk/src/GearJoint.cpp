#include "GearJoint.h"
#include "Body.h"

namespace Chipmunk
{
    GearJoint::GearJoint(std::shared_ptr<Body> bodyA,
                         std::shared_ptr<Body> bodyB,
                         cpFloat phase,
                         cpFloat ratio) :
    Constraint(cpGearJointNew(*bodyA, *bodyB, phase, ratio),
    bodyA, bodyB)
    {}
}