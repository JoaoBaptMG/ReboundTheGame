#ifndef CHIPMUNK_DAMPED_ROTARY_SPRING_H
#define CHIPMUNK_DAMPED_ROTARY_SPRING_H

#include "Constraint.h"
#include <chipmunk.h>

namespace cp
{
    class DampedRotarySpring : public Constraint
    {
    public:
        DampedRotarySpring(std::shared_ptr<Body> bodyA,
                           std::shared_ptr<Body> bodyB,
                           cpFloat restAngle,
                           cpFloat stiffness,
                           cpFloat damping);
        
        /// Get the rest length of the spring.
        inline cpFloat getRestAngle() { return cpDampedRotarySpringGetRestAngle(_constraint); };
        /// Set the rest length of the spring.
        inline void setRestAngle( cpFloat restAngle) { cpDampedRotarySpringSetRestAngle(_constraint, restAngle); };
        
        /// Get the stiffness of the spring in force/distance.
        inline cpFloat getStiffness() { return cpDampedRotarySpringGetStiffness(_constraint); };
        /// Set the stiffness of the spring in force/distance.
        inline void setStiffness(cpFloat stiffness) { cpDampedRotarySpringSetStiffness(_constraint, stiffness); };
        
        /// Get the damping of the spring.
        inline cpFloat getDamping() { return cpDampedRotarySpringGetDamping(_constraint); };
        /// Set the damping of the spring.
        inline void setDamping(cpFloat damping) { cpDampedRotarySpringSetDamping(_constraint, damping); };
    };
}

#endif /* CHIPMUNK_DAMPED_ROTARY_SPRING_H */
