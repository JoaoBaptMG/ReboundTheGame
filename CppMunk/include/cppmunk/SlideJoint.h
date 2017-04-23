#ifndef CHIPMUNK_SLIDE_JOINT_H
#define CHIPMUNK_SLIDE_JOINT_H

#include "Constraint.h"
#include <chipmunk.h>

namespace cp
{
    class SlideJoint : public Constraint
    {
    public:
        SlideJoint(std::shared_ptr<Body> bodyA,
                   std::shared_ptr<Body> bodyB,
                   cpVect anchorA,
                   cpVect anchorB,
                   cpFloat min,
                   cpFloat max);
        
        /// Get the location of the first anchor relative to the first body.
        inline cpVect getAnchorA() { return cpSlideJointGetAnchorA(_constraint); };
        /// Set the location of the first anchor relative to the first body.
        inline void setAnchorA(cpVect anchorA) { cpSlideJointSetAnchorA(_constraint, anchorA); };
        
        /// Get the location of the second anchor relative to the second body.
        inline cpVect getAnchorB() { return cpSlideJointGetAnchorB(_constraint); };
        /// Set the location of the second anchor relative to the second body.
        inline void setAnchorB(cpVect anchorB) { cpSlideJointSetAnchorB(_constraint, anchorB); };
        
        /// Get the minimum distance the joint will maintain between the two anchors.
        inline cpFloat getMin() { return cpSlideJointGetMin(_constraint); };
        /// Set the minimum distance the joint will maintain between the two anchors.
        inline void setMin(cpFloat min) { cpSlideJointSetMin(_constraint, min); };
        
        /// Get the maximum distance the joint will maintain between the two anchors.
        inline cpFloat getMax() { return cpSlideJointGetMax(_constraint); };
        /// Set the maximum distance the joint will maintain between the two anchors.
        inline void setMax(cpFloat max) { cpSlideJointSetMax(_constraint, max); };
    };
}

#endif /* CHIPMUNK_SLIDE_JOINT_H */
