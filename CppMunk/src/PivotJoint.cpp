#include "PivotJoint.h"
#include "Body.h"

namespace cp
{
    PivotJoint::PivotJoint(std::shared_ptr<Body> bodyA,
                           std::shared_ptr<Body> bodyB,
                           cpVect pivot) :
    Constraint(cpPivotJointNew(*bodyA, *bodyB, pivot),
               bodyA, bodyB)
    {}
    
    PivotJoint::PivotJoint(std::shared_ptr<Body> bodyA,
                           std::shared_ptr<Body> bodyB,
                           cpVect anchorA,
                           cpVect anchorB) :
    Constraint(cpPivotJointNew2(*bodyA, *bodyB, anchorA, anchorB),
               bodyA, bodyB)
    {}
}
