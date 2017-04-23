#include "DampedSpring.h"
#include "Body.h"

namespace cp
{
    DampedSpring::DampedSpring(std::shared_ptr<Body> bodyA,
                               std::shared_ptr<Body> bodyB,
                               cpVect anchorA,
                               cpVect anchorB,
                               cpFloat restLength,
                               cpFloat stiffness,
                               cpFloat damping) :
    Constraint(cpDampedSpringNew(*bodyA, *bodyB, anchorA, anchorB, restLength, stiffness, damping),
               bodyA, bodyB)
    {}
}
