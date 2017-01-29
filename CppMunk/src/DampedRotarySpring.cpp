#include "DampedRotarySpring.h"
#include "Body.h"

namespace Chipmunk
{
    DampedRotarySpring::DampedRotarySpring(std::shared_ptr<Body> bodyA,
                                           std::shared_ptr<Body> bodyB,
                                           cpFloat restAngle,
                                           cpFloat stiffness,
                                           cpFloat damping) :
    Constraint(cpDampedRotarySpringNew(*bodyA, *bodyB, restAngle, stiffness, damping),
               bodyA, bodyB)
    {}
}