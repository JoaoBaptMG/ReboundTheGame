#ifndef CHIPMUNK_GROOVE_JOINT_H
#define CHIPMUNK_GROOVE_JOINT_H

#include "Constraint.h"
#include <chipmunk.h>

namespace cp
{
    class GrooveJoint : public Constraint
    {
    public:
        GrooveJoint(std::shared_ptr<Body> bodyA,
                    std::shared_ptr<Body> bodyB,
                    cpVect groove_a,
                    cpVect groove_b,
                    cpVect anchorB);
        
        /// Get the first endpoint of the groove relative to the first body.
        inline cpVect getGrooveA() { return cpGrooveJointGetGrooveA(_constraint); };
        /// Set the first endpoint of the groove relative to the first body.
        inline void setGrooveA(cpVect grooveA) { cpGrooveJointSetGrooveA(_constraint, grooveA); };
        
        /// Get the first endpoint of the groove relative to the first body.
        inline cpVect getGrooveB() { return cpGrooveJointGetGrooveB(_constraint); };
        /// Set the first endpoint of the groove relative to the first body.
        inline void setGrooveB(cpVect grooveB) { cpGrooveJointSetGrooveB(_constraint, grooveB); };
        
        /// Get the location of the second anchor relative to the second body.
        inline cpVect getAnchorB() { return cpGrooveJointGetAnchorB(_constraint); };
        /// Set the location of the second anchor relative to the second body.
        inline void setAnchorB(cpVect anchorB) { cpGrooveJointSetAnchorB(_constraint, anchorB); };
    };
}

#endif /* CHIPMUNK_GROOVE_JOINT_H */
