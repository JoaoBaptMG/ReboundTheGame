#include "SlideJoint.h"
#include "Body.h"

namespace cp
{
    SlideJoint::SlideJoint(std::shared_ptr<Body> bodyA,
                           std::shared_ptr<Body> bodyB,
                           cpVect anchorA,
                           cpVect anchorB,
                           cpFloat min,
                           cpFloat max) :
    Constraint(cpSlideJointNew(*bodyA, *bodyB, anchorA, anchorB, min, max),
               bodyA, bodyB)
    {}
}

