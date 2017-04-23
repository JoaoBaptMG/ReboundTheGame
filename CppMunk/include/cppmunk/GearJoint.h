#ifndef CHIPMUNK_GEAR_JOINT_H
#define CHIPMUNK_GEAR_JOINT_H

#include "Constraint.h"
#include <chipmunk.h>

namespace cp
{
    class GearJoint : public Constraint
    {
    public:
        GearJoint(std::shared_ptr<Body> bodyA,
                  std::shared_ptr<Body> bodyB,
                  cpFloat phase,
                  cpFloat ratio);
        
        /// Get the phase offset of the gears.
        inline cpFloat getPhase() { return cpGearJointGetPhase(_constraint); };
        /// Set the phase offset of the gears.
        inline void setPhase(cpFloat phase) { cpGearJointSetPhase(_constraint, phase); };
        
        /// Get the angular distance of each ratchet.
        inline cpFloat getRatio() { return cpGearJointGetRatio(_constraint); };
        /// Set the ratio of a gear joint.
        inline void setRatio(cpFloat ratio) { cpGearJointSetRatio(_constraint, ratio); };
    };
}

#endif /* CHIPMUNK_GEAR_JOINT_H */
