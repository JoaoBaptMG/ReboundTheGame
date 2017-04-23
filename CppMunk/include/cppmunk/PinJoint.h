#ifndef CHIPMUNK_PIN_JOINT_H
#define CHIPMUNK_PIN_JOINT_H

#include "Constraint.h"
#include <chipmunk.h>

namespace cp
{
    class PinJoint : public Constraint
    {
    public:
        PinJoint(std::shared_ptr<Body> bodyA,
                 std::shared_ptr<Body> bodyB,
                 cpVect anchorA,
                 cpVect anchorB);
        
        /// Get the location of the first anchor relative to the first body.
        inline cpVect getAnchorA() { return cpPinJointGetAnchorA(_constraint); }
        /// Set the location of the first anchor relative to the first body.
        inline void setAnchorA(cpVect anchorA) { cpPinJointSetAnchorA(_constraint, anchorA); };
        
        /// Get the location of the second anchor relative to the second body.
        inline cpVect getAnchorB() { return cpPinJointGetAnchorB(_constraint); }
        /// Set the location of the second anchor relative to the second body.
        inline void setAnchorB(cpVect anchorB) { cpPinJointSetAnchorB(_constraint, anchorB); };
        
        /// Get the distance the joint will maintain between the two anchors.
        inline cpFloat getDist() { return cpPinJointGetDist(_constraint); };
        /// Set the distance the joint will maintain between the two anchors.
        inline void setDist(cpFloat dist) { cpPinJointSetDist(_constraint, dist); }
    };
}

#endif /* CHIPMUNK_PIN_JOINT_H */
