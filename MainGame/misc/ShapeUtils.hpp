#pragma once

#include <cppmunk/Shape.h>
#include <cppmunk/Constraint.h>
#include <memory>

std::shared_ptr<cp::Shape> cloneShape(const std::shared_ptr<cp::Shape>& shape);
void flipShapeHorizontally(std::shared_ptr<cp::Shape>& shape);

std::shared_ptr<cp::Constraint> cloneConstraint(const std::shared_ptr<cp::Constraint>& constraint);
void flipConstraintHorizontally(std::shared_ptr<cp::Constraint>& constraint);
