#include "Constraint.h"

namespace cp
{
    Constraint::Constraint(cpConstraint* constraint,
                           std::shared_ptr<Body> bodyA,
                           std::shared_ptr<Body> bodyB) :
    _constraint(constraint),
    _bodyA(bodyA),
    _bodyB(bodyB)
    { }
    
    Constraint::~Constraint()
    {
        if (_constraint != NULL)
            cpConstraintFree(_constraint);
    }
    
    Constraint::operator cpConstraint*() const
    {
        return _constraint;
    }
}
