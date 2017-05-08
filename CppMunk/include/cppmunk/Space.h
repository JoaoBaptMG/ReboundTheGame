#ifndef CHIPMUNK_SPACE_H
#define CHIPMUNK_SPACE_H

#include <chipmunk/chipmunk.h>
#include "LayerMask.h"
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>

namespace cp
{
    class Body;
    class Arbiter;

    class Constraint;
    class Shape;

    typedef std::function<void(cpShape*, cpVect, cpFloat, cpVect)> PointQueryFunc;
    typedef std::function<void(std::shared_ptr<Shape>, cpFloat, cpVect)> SegmentQueryFunc;
    
    class Space
    {
    public:
        Space();
        explicit Space(cpSpace*);
        ~Space();
        operator cpSpace*();
        
        /// Number of iterations to use in the impulse solver to solve contacts and other constraints.
        inline int getIterations() const { return cpSpaceGetIterations(_space); };
        inline void setIterations(int iterations) { cpSpaceSetIterations(_space, iterations); };

        /// Gravity to pass to rigid bodies when integrating velocity.
        inline cpVect getGravity() const { return cpSpaceGetGravity(_space); };
        inline void setGravity(cpVect gravity) { cpSpaceSetGravity(_space, gravity); };

        /// Damping rate expressed as the fraction of velocity bodies retain each second.
        /// A value of 0.9 would mean that each body's velocity will drop 10% per second.
        /// The default value is 1.0, meaning no damping is applied.
        /// @note This damping value is different than those of cpDampedSpring and cpDampedRotarySpring.
        inline cpFloat getDamping() const { return cpSpaceGetDamping(_space); };
        inline void setDamping(cpFloat damping) { cpSpaceSetDamping(_space, damping); };

        /// Speed threshold for a body to be considered idle.
        /// The default value of 0 means to let the space guess a good threshold based on gravity.
        inline cpFloat getIdleSleepThreshold() { return cpSpaceGetIdleSpeedThreshold(_space); };
        inline void setIdleSpeedThreshold(cpFloat idleSpeedThreshold) { cpSpaceSetIdleSpeedThreshold(_space, idleSpeedThreshold); };
        
        /// Time a group of bodies must remain idle in order to fall asleep.
        /// Enabling sleeping also implicitly enables the the contact graph.
        /// The default value of INFINITY disables the sleeping algorithm.
        inline cpFloat getSleepTimeThreshold() { return cpSpaceGetSleepTimeThreshold(_space); };
        inline void setSleepTimeThreshold(cpFloat sleepTimeThreshold) { cpSpaceSetSleepTimeThreshold(_space, sleepTimeThreshold); };

        /// Amount of encouraged penetration between colliding shapes.
        /// Used to reduce oscillating contacts and keep the collision cache warm.
        /// Defaults to 0.1. If you have poor simulation quality,
        /// increase this number as much as possible without allowing visible amounts of overlap.
        inline cpFloat getCollisionSlop() { return cpSpaceGetCollisionSlop(_space); };
        inline void setCollisionSlop(cpFloat collisionSlop) { cpSpaceSetCollisionSlop(_space, collisionSlop); };
        
        /// Determines how fast overlapping shapes are pushed apart.
        /// Expressed as a fraction of the error remaining after each second.
        /// Defaults to pow(1.0 - 0.1, 60.0) meaning that Chipmunk fixes 10% of overlap each frame at 60Hz.
        inline cpFloat getCollisionBias() { return cpSpaceGetCollisionBias(_space); };
        inline void setCollisionBias(cpFloat collisionBias) { cpSpaceSetCollisionBias(_space, collisionBias); };

        /// Number of frames that contact information should persist.
        /// Defaults to 3. There is probably never a reason to change this value.
        inline cpTimestamp getCollisionPresistence() { return cpSpaceGetCollisionPersistence(_space); };
        inline void setCollisionPersistence(cpTimestamp collisionPersistence) { cpSpaceSetCollisionPersistence(_space, collisionPersistence); };

        /// User definable data pointer.
        /// Generally this points to your game's controller or game state
        /// class so you can access it when given a cpSpace reference in a callback.
        inline cpDataPointer getUserData() { return cpSpaceGetUserData(_space); };
        inline void setUserData(cpDataPointer userData) { cpSpaceSetUserData(_space, userData); };

        /// The Space provided static body for a given cpSpace.
        /// This is merely provided for convenience and you are not required to use it.
        std::shared_ptr<Body> getStaticBody() { return _staticBody; };

        /// Returns the current (or most recent) time step used with the given space.
        /// Useful from callbacks if your time step is not a compile-time global.
        inline cpFloat getTimeStamp() const { return cpSpaceGetCurrentTimeStep(_space); };

        /// returns true from inside a callback when objects cannot be added/removed.
        inline cpBool isLocked() { return cpSpaceIsLocked(_space); };

        /// Create a collision handler for the specified pair of collision types.
        /// If wildcard handlers are used with either of the collision types, it's the responibility of the custom handler to invoke the wildcard handlers.
        void addCollisionHandler(cpCollisionType a, cpCollisionType b,
                                 std::function<int(Arbiter, Space&)> begin,
                                 std::function<int(Arbiter, Space&)> preSolve,
                                 std::function<void(Arbiter, Space&)> postSolve,
                                 std::function<void(Arbiter, Space&)> separate);

        /// Create a wildcard collision handler for the specified collision type.
        void addWildcardCollisionHandler(cpCollisionType a,
                                         std::function<int(Arbiter, Space&)> begin,
                                         std::function<int(Arbiter, Space&)> preSolve,
                                         std::function<void(Arbiter, Space&)> postSolve,
                                         std::function<void(Arbiter, Space&)> separate);
        
        /// Add a collision shape to the simulation.
        /// If the shape is attached to a static body, it will be added as a static shape.
        void add(std::shared_ptr<Shape>);
        /// Add a rigid body to the simulation.
        void add(std::shared_ptr<Body>);
        /// Add a constraint to the simulation.
        void add(std::shared_ptr<Constraint>);

        /// Remove a collision shape from the simulation.
        void remove(std::shared_ptr<Shape>);
        /// Remove a rigid body from the simulation.
        void remove(std::shared_ptr<Body>);
        /// Remove a constraint from the simulation.
        void remove(std::shared_ptr<Constraint>);

        /// Query the space at a point and call @c for each shape found which is on a reasonable distance
        void pointQuery(cpVect point, cpFloat maxDistance, cpShapeFilter, PointQueryFunc) const;
        /// Query the space at a point and return the nearest shape found. Returns NULL if no shapes were found.
        std::shared_ptr<Shape> pointQueryNearest(cpVect p, LayerMask, cpGroup) const;
        
        /// Perform a directed line segment query (like a raycast) against the space calling @c func for each shape intersected.
        void segmentQuery(cpVect a, cpVect b, LayerMask, cpGroup, SegmentQueryFunc) const;
        /// Perform a directed line segment query (like a raycast) against the space and return the first shape hit. Returns NULL if no shapes were hit.
        std::shared_ptr<Shape> segmentQueryFirst(cpVect a, cpVect b, LayerMask, cpGroup, cpSegmentQueryInfo* = nullptr) const;

        /// Update the collision detection info for the static shapes in the space.
        void reindexStatic() { cpSpaceReindexStatic(_space); };
        /// Update the collision detection data for a specific shape in the space.
        void reindexShape(std::shared_ptr<Shape> shape);
        /// Update the collision detection data for all shapes attached to a body.
        void reindexShapesForBody(std::shared_ptr<Body> body);
        
        /// Step the space forward in time by @c dt.
        virtual void step(cpFloat dt);

        void addPostStepCallback(void* key, std::function<void()> callback);

        // Remove all shapes, bodies and constraints in the space
        virtual void clearSpace();
        
        inline cpSpace* getSpace() const { return _space; };
    protected:
        cpSpace* _space;
        std::shared_ptr<Body> _staticBody;
        
    private:
        Space(const Space&);
        const Space& operator=(const Space&);
        std::shared_ptr<Shape> findShape(cpShape*) const;
        std::shared_ptr<Body> findBody(cpBody*) const;
        std::shared_ptr<Constraint> findConstraint(cpConstraint*) const;

        std::unordered_map<cpShape*, std::shared_ptr<Shape>> _shapes;
        std::unordered_map<cpBody*, std::shared_ptr<Body>> _bodies;
        std::unordered_map<cpConstraint*, std::shared_ptr<Constraint>> _constraints;

        struct SegmentQueryData
        {
            const Space* const self;
            SegmentQueryFunc& func;
        };
        
        struct CallbackData
        {
            std::function<int(Arbiter, Space&)> begin;
            std::function<int(Arbiter, Space&)> preSolve;
            std::function<void(Arbiter, Space&)> postSolve;
            std::function<void(Arbiter, Space&)> separate;
            Space* self;
            
            CallbackData(std::function<int(Arbiter, Space&)> begin, std::function<int(Arbiter, Space&)> preSolve,
                         std::function<void(Arbiter, Space&)> postSolve, std::function<void(Arbiter, Space&)> separate,
                         Space& self)
            : begin(begin), preSolve(preSolve), postSolve(postSolve), separate(separate), self(&self)
            {}
            CallbackData() {}
        };
        
        std::map<std::pair<cpCollisionType, cpCollisionType>, CallbackData> callbackDatas;
        std::map<cpCollisionType, CallbackData> wildcardCallbackDatas;
        
        static cpBool helperBegin(cpArbiter* arb, cpSpace* s, void* d);
        static cpBool helperPreSolve(cpArbiter* arb, cpSpace* s, void* d);
        static void helperPostSolve(cpArbiter* arb, cpSpace* s, void* d);
        static void helperSeparate(cpArbiter* arb, cpSpace* s, void* d);
    };
}


#endif /* CHIPMUNK_SPACE_H */
