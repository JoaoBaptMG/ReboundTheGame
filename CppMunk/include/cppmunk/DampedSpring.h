#ifndef CHIPMUNK_DAMPED_SPRING_H
#define CHIPMUNK_DAMPED_SPRING_H

#include "Constraint.h"
#include <chipmunk/chipmunk.h>

namespace cp
{
    class DampedSpring : public Constraint
    {
    public:
        DampedSpring(std::shared_ptr<Body> bodyA,
                     std::shared_ptr<Body> bodyB,
                     cpVect anchorA,
                     cpVect anchorB,
                     cpFloat restLength,
                     cpFloat stiffness,
                     cpFloat damping);
        
        /// Get the location of the first anchor relative to the first body.
        inline cpVect getAnchorA() { return cpDampedSpringGetAnchorA(_constraint); };
        /// Set the location of the first anchor relative to the first body.
        inline void setAnchorA(cpVect anchorA) { cpDampedSpringSetAnchorA(_constraint, anchorA); };
        
        /// Get the location of the second anchor relative to the second body.
        inline cpVect getAnchorB() { return cpDampedSpringGetAnchorB(_constraint); };
        /// Set the location of the second anchor relative to the second body.
        inline void setAnchorB(cpVect anchorB) { cpDampedSpringSetAnchorA(_constraint, anchorB); };
        
        /// Get the rest length of the spring.
        inline cpFloat getRestLength() { return cpDampedSpringGetRestLength(_constraint); };
        /// Set the rest length of the spring.
        inline void setRestLength(cpFloat restLength) { cpDampedSpringSetRestLength(_constraint, restLength); }
        
        /// Get the stiffness of the spring in force/distance.
        inline cpFloat getStiffness() { return cpDampedSpringGetStiffness(_constraint); };
        /// Set the stiffness of the spring in force/distance.
        inline void setStiffness(cpFloat stiffness) { cpDampedSpringSetStiffness(_constraint, stiffness); }
        
        /// Get the damping of the spring.
        inline cpFloat getDamping() { return cpDampedSpringGetDamping(_constraint); };
        /// Set the damping of the spring.
        inline void setDamping(cpFloat damping) { cpDampedSpringSetDamping(_constraint, damping); };
    };
}

#endif /* CHIPMUNK_DAMPED_SPRING_H */
