#include "SimpleMotor.h"
#include "Body.h"

namespace Chipmunk
{
    SimpleMotor::SimpleMotor(std::shared_ptr<Body> bodyA,
                             std::shared_ptr<Body> bodyB,
                             cpFloat rate) :
    Constraint(cpSimpleMotorNew(*bodyA, *bodyB, rate),
               bodyA, bodyB)
    { }
}

