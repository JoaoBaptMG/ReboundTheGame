#ifndef CHIPMUNK_ROTARY_LIMIT_JOINT_H
#define CHIPMUNK_ROTARY_LIMIT_JOINT_H

#include "Constraint.h"
#include <chipmunk.h>

namespace cp
{
    class RotaryLimitJoint : public Constraint
    {
    public:
        RotaryLimitJoint(std::shared_ptr<Body> bodyA,
                         std::shared_ptr<Body> bodyB,
                         cpFloat min,
                         cpFloat max);

        /// Get the minimum distance the joint will maintain between the two anchors.
        inline cpFloat getMin() { return cpRotaryLimitJointGetMin(_constraint); };
        /// Set the minimum distance the joint will maintain between the two anchors.
        inline void setMin(cpFloat min) { cpRotaryLimitJointSetMin(_constraint, min); }
        
        /// Get the maximum distance the joint will maintain between the two anchors.
        inline cpFloat getMax() { return cpRotaryLimitJointGetMax(_constraint); };
        /// Set the maximum distance the joint will maintain between the two anchors.
        inline void setMax(cpFloat max) { cpRotaryLimitJointSetMax(_constraint, max); };
    };
}

#endif /* CHIPMUNK_ROTARY_LIMIT_JOINT_H */
