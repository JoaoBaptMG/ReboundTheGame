#ifndef CHIPMUNK_PIVOT_JOINT_H
#define CHIPMUNK_PIVOT_JOINT_H

#include "Constraint.h"
#include <chipmunk/chipmunk.h>

namespace cp
{
    class PivotJoint : public Constraint
    {
    public:
        PivotJoint(std::shared_ptr<Body> bodyA,
                   std::shared_ptr<Body> bodyB,
                   cpVect pivot);
        
        PivotJoint(std::shared_ptr<Body> bodyA,
                   std::shared_ptr<Body> bodyB,
                   cpVect anchorA,
                   cpVect anchorB);
        
        /// Get the location of the first anchor relative to the first body.
        inline cpVect getAnchorA() { return cpPivotJointGetAnchorA(_constraint); };
        /// Set the location of the first anchor relative to the first body.
        inline void setAnchorA(cpVect anchorA) { cpPivotJointSetAnchorA(_constraint, anchorA); };
        
        /// Get the location of the second anchor relative to the second body.
        inline cpVect getAnchorB() { return cpPivotJointGetAnchorB(_constraint); };
        /// Set the location of the second anchor relative to the second body.
        inline void setAnchorB(cpVect anchorB) { cpPivotJointSetAnchorB(_constraint, anchorB); }
    };
}

#endif /* CHIPMUNK_PIVOT_JOINT_H */
