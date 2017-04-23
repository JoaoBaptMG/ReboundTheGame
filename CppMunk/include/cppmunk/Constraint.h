#ifndef CHIPMUNK_CONSTRAINT_H
#define CHIPMUNK_CONSTRAINT_H

#include <chipmunk.h>
#include <memory>

namespace cp
{
    class Body;

    class Constraint
    {
    public:
        virtual ~Constraint();
        operator cpConstraint*() const;
        
        /// Get the first body the constraint is attached to.
        std::shared_ptr<Body> getBodyA() { return _bodyA; }
        /// Get the second body the constraint is attached to.
        std::shared_ptr<Body> getBodyB() { return _bodyB; }

        /// Get the maximum force that this constraint is allowed to use.
        inline cpFloat getMaxForce() { return cpConstraintGetMaxForce(_constraint); };
        /// Set the maximum force that this constraint is allowed to use. (defaults to INFINITY)
        inline void setMaxForce(cpFloat data) { cpConstraintSetMaxForce(_constraint, data); };

        /// Get rate at which joint error is corrected.
        inline cpFloat getErrorBias() { return cpConstraintGetErrorBias(_constraint); };
        /// Set rate at which joint error is corrected.
        /// Defaults to pow(1.0 - 0.1, 60.0) meaning that it will
        /// correct 10% of the error every 1/60th of a second.
        inline void setErrorBias(cpFloat errorBias) { cpConstraintSetErrorBias(_constraint, errorBias); };
        
        /// Get the maximum rate at which joint error is corrected.
        inline cpFloat getMaxBias() { return cpConstraintGetMaxBias(_constraint); };
        /// Set the maximum rate at which joint error is corrected. (defaults to INFINITY)
        inline void setMaxBias(cpFloat data) { cpConstraintSetMaxBias(_constraint, data); };

        /// Get if the two bodies connected by the constraint are allowed to collide or not.
        inline cpBool getCollideBodies() { return cpConstraintGetCollideBodies(_constraint); };
        /// Set if the two bodies connected by the constraint are allowed to collide or not. (defaults to cpFalse)
        inline void setCollideBodies(cpBool collideBodies) { cpConstraintSetCollideBodies(_constraint, collideBodies); };

        /// Get the user definable data pointer for this constraint
        inline cpDataPointer getUserData() { return cpConstraintGetUserData(_constraint); };
        /// Set the user definable data pointer for this constraint
        inline void setUserData(cpDataPointer data) { cpConstraintSetUserData(_constraint, data); };

        /// Get the last impulse applied by this constraint.
        inline cpFloat getImpulse() { return cpConstraintGetImpulse(_constraint); };

    protected:
        Constraint(cpConstraint* constraint,
                   std::shared_ptr<Body> bodyA,
                   std::shared_ptr<Body> bodyB);

        cpConstraint* _constraint;
        
        std::shared_ptr<Body> _bodyA;
        std::shared_ptr<Body> _bodyB;
        void setBodyA(std::shared_ptr<Body> b);
        void setBodyB(std::shared_ptr<Body> b);

    private:
        Constraint(const Constraint&);
        const Constraint& operator=(const Constraint&);

    };
}

#endif /* CHIPMUNK_CONSTRAINT_H */
