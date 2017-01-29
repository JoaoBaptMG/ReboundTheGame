#ifndef CHIPMUNK_SIMPLE_MOTOR_H
#define CHIPMUNK_SIMPLE_MOTOR_H

#include "Constraint.h"
#include <chipmunk.h>

namespace Chipmunk
{
    class SimpleMotor : public Constraint
    {
    public:
        SimpleMotor(std::shared_ptr<Body> bodyA,
                    std::shared_ptr<Body> bodyB,
                    cpFloat rate);
        
        /// Get the rate of the motor.
        inline cpFloat getRate() { return cpSimpleMotorGetRate(_constraint); };
        /// Set the rate of the motor.
        inline void setRate(cpFloat rate) { cpSimpleMotorSetRate(_constraint, rate); };
    };
}

#endif /* CHIPMUNK_SIMPLE_MOTOR_H */
