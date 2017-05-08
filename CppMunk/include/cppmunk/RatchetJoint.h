#ifndef CHIPMUNK_RATCHET_JOINT_H
#define CHIPMUNK_RATCHET_JOINT_H

#include "Constraint.h"
#include <chipmunk/chipmunk.h>

namespace cp
{
    class RatchetJoint : public Constraint
    {
    public:
        RatchetJoint(std::shared_ptr<Body> bodyA,
                     std::shared_ptr<Body> bodyB,
                     cpFloat phase,
                     cpFloat ratchet);
        
        /// Get the angle of the current ratchet tooth.
        inline cpFloat getAngle() { return cpRatchetJointGetAngle(_constraint); };
        /// Set the angle of the current ratchet tooth.
        inline void setAngle(cpFloat angle) { cpRatchetJointSetAngle(_constraint, angle); };
        
        /// Get the phase offset of the ratchet.
        inline cpFloat getPhase() { return cpRatchetJointGetPhase(_constraint); };
        /// Get the phase offset of the ratchet.
        inline void setPhase(cpFloat phase) { cpRatchetJointSetPhase(_constraint, phase); };
        
        /// Get the angular distance of each ratchet.
        inline cpFloat getRatchet() { return cpRatchetJointGetRatchet(_constraint); };
        /// Set the angular distance of each ratchet.
        inline void setRatchet(cpFloat ratchet) { cpRatchetJointSetRatchet(_constraint, ratchet); };
    };
}

#endif /* CHIPMUNK_RATCHET_JOINT_H */
