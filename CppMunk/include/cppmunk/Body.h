#ifndef CHIPMUNK_BODY_H
#define CHIPMUNK_BODY_H

#include <chipmunk.h>
#include <memory>

namespace Chipmunk
{
    class Shape;
    class Space;
    
    class Body
    {
    public:
        Body(cpFloat mass, cpFloat inertia);
        Body(Body&&);
        explicit Body(cpBody* body);
        ~Body();
        operator cpBody*() const;

        /// Wake up a sleeping or idle body.
        inline void activate() { cpBodyActivate(_body); };
        /// Wake up any sleeping or idle bodies touching a static body.
        inline void activateStatic(std::shared_ptr<Shape> filter);
        
        /// Force a body to fall asleep immediately.
        inline void sleep() { cpBodySleep(_body); };
        /// Force a body to fall asleep immediately along with other bodies in a group.
        inline void sleepWithGroup(std::shared_ptr<Body> group) { cpBodySleepWithGroup(_body, *group); };
        
        /// Returns true if the body is sleeping.
        inline cpBool isSleeping() { return cpBodyIsSleeping(_body); };

        /// Get the type of the body.
        inline cpBodyType getBodyType() const { return cpBodyGetType(_body); };
        /// Set the type of the body.
        inline void setBodyType(cpBodyType type) { cpBodySetType(_body, type); }

        /// Get the mass of the body.
        inline cpFloat getMass() const { return cpBodyGetMass(_body); };
        /// Set the mass of the body.
        inline void setMass(cpFloat mass) { cpBodySetMass(_body, mass); };

        /// Get the moment of inertia of the body.
        inline cpFloat getMoment() const { return cpBodyGetMoment(_body); };
        /// Set the moment of inertia of the body.
        inline void setMoment(cpFloat moment) { cpBodySetMoment(_body, moment); };
        
        /// Set the position of a body.
        inline cpVect getPosition() const { return cpBodyGetPosition(_body); };
        /// Set the position of the body.
        inline void setPosition(cpVect position) { cpBodySetPosition(_body, position); };
        
        /// Get the offset of the center of gravity in body local coordinates.
        inline cpVect getCenterOfGravity() { return cpBodyGetCenterOfGravity(_body); };
        /// Set the offset of the center of gravity in body local coordinates.
        inline void setCenterOfGravity(cpVect cog) { cpBodySetCenterOfGravity(_body, cog); };
        
        /// Get the velocity of the body.
        inline cpVect getVelocity() const { return cpBodyGetVelocity(_body); };
        /// Set the velocity of the body.
        inline void setVelocity(cpVect velocity) { cpBodySetVelocity(_body, velocity); };

        /// Get the force applied to the body for the next time step.
        inline cpVect getForce() const { return cpBodyGetForce(_body); };
        /// Set the force applied to the body for the next time step.
        inline void setAngle(cpVect force) { cpBodySetForce(_body, force); };

        /// Get the angle of the body.
        inline cpFloat getAngle() const { return cpBodyGetAngle(_body); };
        /// Set the angle of a body.
        inline void setAngle(cpFloat angle) { cpBodySetAngle(_body, angle); };
        
        /// Get the angular velocity of the body.
        inline cpFloat getAngularVelocity() const { return cpBodyGetAngularVelocity(_body); };
        /// Set the angular velocity of the body.
        inline void setAngularVelocity(cpFloat velocity) { cpBodySetAngularVelocity(_body, velocity); };
        
        /// Get the torque applied to the body for the next time step.
        inline cpFloat getTorque() const { return cpBodyGetTorque(_body); };
        /// Set the torque applied to the body for the next time step.
        inline void setTorque(cpFloat torque) { cpBodySetTorque(_body, torque); };
        
        /// Get the rotation vector of the body. (The x basis vector of it's transform.)
        inline cpVect getRotation() const { return cpBodyGetRotation(_body); };
        
        /// Get the user data pointer assigned to the body.
        inline cpDataPointer getUserData() const { return cpBodyGetUserData(_body); }
        /// Set the user data pointer assigned to the body.
        inline void setUserData(cpDataPointer data) { cpBodySetUserData(_body, data); }

        /// Default velocity integration function..
        void updateVelocity(cpVect gravity, cpFloat damping, cpFloat dt) { cpBodyUpdateVelocity(_body, gravity, damping, dt); };
        /// Default position integration function.
        void updatePosition(cpFloat dt) { cpBodyUpdatePosition(_body, dt); };
        
        /// Convert body relative/local coordinates to absolute/world coordinates.
        inline cpVect localToWorld(cpVect point) const { return cpBodyLocalToWorld(_body, point); };
        /// Convert body absolute/world coordinates to  relative/local coordinates.
        inline cpVect worldToLocal(cpVect point) const { return cpBodyWorldToLocal(_body, point); };
        
        /// Apply a force to a body. Both the force and point are expressed in world coordinates.
        void applyForceAtWorldPoint(cpVect force, cpVect point) { cpBodyApplyForceAtWorldPoint(_body, force, point); };
        /// Apply a force to a body. Both the force and point are expressed in body local coordinates.
        void applyForceAtLocalPoint(cpVect force, cpVect point) { cpBodyApplyForceAtWorldPoint(_body, force, point); };

        /// Apply an impulse to a body. Both the impulse and point are expressed in world coordinates.
        void applyImpulseAtWorldPoint(cpVect impulse, cpVect point) { cpBodyApplyImpulseAtWorldPoint(_body, impulse, point); };
        /// Apply an impulse to a body. Both the impulse and point are expressed in body local coordinates.
        void applyImpulseAtLocalPoint(cpVect impulse, cpVect point) { cpBodyApplyImpulseAtLocalPoint(_body, impulse, point); };
        
        /// Get the velocity on a body (in world units) at a point on the body in world coordinates.
        inline cpVect getVelocityAtWorldPoint(cpVect point) const { return cpBodyGetVelocityAtWorldPoint(_body, point); };
        /// Get the velocity on a body (in world units) at a point on the body in local coordinates.
        inline cpVect getVelocityAtLocalPoint(cpVect point) const { return cpBodyGetVelocityAtLocalPoint(_body, point); };

        /// Get the amount of kinetic energy contained by the body.
        inline cpFloat kineticEnergy() { return cpBodyKineticEnergy(_body); };

    protected:
        cpBody* _body;
        bool _dirty;
        
    private:
        Body(const Body&);
        const Body& operator=(const Body&);

        friend class Space;
    };
}

#endif /* Body_hpp */
