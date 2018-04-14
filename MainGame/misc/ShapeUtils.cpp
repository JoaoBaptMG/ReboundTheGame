#include "ShapeUtils.hpp"

#include <chipmunk/chipmunk_unsafe.h>
#include <chipmunk/chipmunk_structs.h>

#include <cppmunk/CircleShape.h>
#include <cppmunk/SegmentShape.h>
#include <cppmunk/PolyShape.h>

#include <cppmunk/PinJoint.h>
#include <cppmunk/SlideJoint.h>
#include <cppmunk/PivotJoint.h>
#include <cppmunk/GrooveJoint.h>
#include <cppmunk/DampedSpring.h>
#include <cppmunk/DampedRotarySpring.h>
#include <cppmunk/RotaryLimitJoint.h>
#include <cppmunk/RatchetJoint.h>
#include <cppmunk/GearJoint.h>
#include <cppmunk/SimpleMotor.h>

#include <algorithm>

cpVect cpvfliphor(cpVect v) { return cpv(-v.x, v.y); }

std::shared_ptr<cp::Shape> cloneShape(const std::shared_ptr<cp::Shape>& shape)
{
    if (!shape) return std::shared_ptr<cp::Shape>();
    std::shared_ptr<cp::Shape> shp;
    
    switch (((cpShape*)*shape)->klass->type)
    {
        case CP_CIRCLE_SHAPE:
        {
            auto circle = static_cast<cp::CircleShape*>(shape.get());
            shp = std::make_shared<cp::CircleShape>(circle->getBody(), circle->getRadius(), circle->getOffset());
        } break;
        case CP_SEGMENT_SHAPE:
        {
            auto seg = static_cast<cp::SegmentShape*>(shape.get());
            shp = std::make_shared<cp::SegmentShape>(seg->getBody(), seg->getA(), seg->getB(), seg->getRadius());
        } break;
        case CP_POLY_SHAPE:
        {
            auto poly = static_cast<cp::PolyShape*>(shape.get());
            std::vector<cpVect> points(poly->getCount());
            for (int i = 0; i < points.size(); i++)
                points[i] = poly->getVert(i);
            shp = std::make_shared<cp::PolyShape>(poly->getBody(), points, poly->getRadius());
        } break;
        default: return std::shared_ptr<cp::Shape>();
    }
    
    shp->setSensor(shape->isSensor());
    shp->setElasticity(shape->getElasticity());
    shp->setFriction(shape->getFriction());
    shp->setSurfaceVelocity(shape->getSurfaceVelocity());
    shp->setCollisionType(shape->getCollisionType());
    shp->setFilter(shape->getFilter());
    shp->setUserData(shape->getUserData());
    
    return shp;
}

void flipShapeHorizontally(std::shared_ptr<cp::Shape>& shape)
{
    if (!shape) return;
    
    switch (((cpShape*)*shape)->klass->type)
    {
        case CP_CIRCLE_SHAPE:
        {
            auto circle = static_cast<cp::CircleShape*>(shape.get());
            auto ofs = circle->getOffset();
            cpCircleShapeSetOffset(*circle, cpv(-ofs.x, ofs.y));
        } break;
        case CP_SEGMENT_SHAPE:
        {
            auto seg = static_cast<cp::SegmentShape*>(shape.get());
            auto a = seg->getA(), b = seg->getB();
            cpSegmentShapeSetEndpoints(*seg, cpv(-a.x, a.y), cpv(-b.x, b.y));
        } break;
        case CP_POLY_SHAPE:
        {
            auto poly = static_cast<cp::PolyShape*>(shape.get());
            std::vector<cpVect> points(poly->getCount());
            for (int i = 0; i < points.size(); i++)
            {
                points[i] = poly->getVert(i);
                points[i].x = -points[i].x;
            }
            cpPolyShapeSetVertsRaw(*poly, points.size(), points.data());
        } break;
        default: break;
    }
}

std::shared_ptr<cp::Constraint> cloneConstraint(const std::shared_ptr<cp::Constraint>& constraint)
{
    std::shared_ptr<cp::Constraint> res;
    
    if (auto pin = dynamic_cast<cp::PinJoint*>(constraint.get()))
        res = std::make_shared<cp::PinJoint>(pin->getBodyA(), pin->getBodyB(), pin->getAnchorA(), pin->getAnchorB());
    else if (auto slide = dynamic_cast<cp::SlideJoint*>(constraint.get()))
        res = std::make_shared<cp::SlideJoint>(slide->getBodyA(), slide->getBodyB(),
            slide->getAnchorA(), slide->getAnchorB(), slide->getMin(), slide->getMax());
    else if (auto pivot = dynamic_cast<cp::PivotJoint*>(constraint.get()))
        res = std::make_shared<cp::PivotJoint>(pivot->getBodyA(), pivot->getBodyB(), pivot->getAnchorA(), pivot->getAnchorB());
    else if (auto groove = dynamic_cast<cp::GrooveJoint*>(constraint.get()))
        res = std::make_shared<cp::GrooveJoint>(groove->getBodyA(), groove->getBodyB(),
            groove->getGrooveA(), groove->getGrooveB(), groove->getAnchorB());
    else if (auto spring = dynamic_cast<cp::DampedSpring*>(constraint.get()))
        res = std::make_shared<cp::DampedSpring>(spring->getBodyA(), spring->getBodyB(),
            spring->getAnchorA(), spring->getAnchorB(), spring->getRestLength(), spring->getStiffness(),
            spring->getDamping());
    else if (auto spring = dynamic_cast<cp::DampedRotarySpring*>(constraint.get()))
        res = std::make_shared<cp::DampedRotarySpring>(spring->getBodyA(), spring->getBodyB(),
            spring->getRestAngle(), spring->getStiffness(), spring->getDamping());
    else if (auto joint = dynamic_cast<cp::RotaryLimitJoint*>(constraint.get()))
        res = std::make_shared<cp::RotaryLimitJoint>(joint->getBodyA(), joint->getBodyB(), joint->getMin(), joint->getMax());
    else if (auto ratchet = dynamic_cast<cp::RatchetJoint*>(constraint.get()))
        res = std::make_shared<cp::RatchetJoint>(ratchet->getBodyA(), ratchet->getBodyB(), ratchet->getPhase(), ratchet->getRatchet());
    else if (auto gear = dynamic_cast<cp::GearJoint*>(constraint.get()))
        res = std::make_shared<cp::GearJoint>(gear->getBodyA(), gear->getBodyB(), gear->getPhase(), gear->getRatio());
    else if (auto motor = dynamic_cast<cp::SimpleMotor*>(constraint.get()))
        res = std::make_shared<cp::SimpleMotor>(motor->getBodyA(), motor->getBodyB(), motor->getRate());
    else return std::shared_ptr<cp::Constraint>();
    
    res->setMaxForce(constraint->getMaxForce());
    res->setErrorBias(constraint->getErrorBias());
    res->setMaxBias(constraint->getMaxBias());
    res->setCollideBodies(constraint->getCollideBodies());
    
    return res;
}

void flipConstraintHorizontally(std::shared_ptr<cp::Constraint>& constraint)
{
    if (auto pin = dynamic_cast<cp::PinJoint*>(constraint.get()))
    {
        pin->setAnchorA(cpvfliphor(pin->getAnchorA()));
        pin->setAnchorB(cpvfliphor(pin->getAnchorB()));
    }
    else if (auto slide = dynamic_cast<cp::SlideJoint*>(constraint.get()))
    {
        slide->setAnchorA(cpvfliphor(slide->getAnchorA()));
        slide->setAnchorB(cpvfliphor(slide->getAnchorB()));
    }
    else if (auto pivot = dynamic_cast<cp::PivotJoint*>(constraint.get()))
    {
        pivot->setAnchorA(cpvfliphor(pivot->getAnchorA()));
        pivot->setAnchorB(cpvfliphor(pivot->getAnchorB()));
    }
    else if (auto groove = dynamic_cast<cp::GrooveJoint*>(constraint.get()))
    {
        groove->setGrooveA(cpvfliphor(groove->getGrooveA()));
        groove->setGrooveB(cpvfliphor(groove->getGrooveB()));
        groove->setAnchorB(cpvfliphor(groove->getAnchorB()));
    }
    else if (auto spring = dynamic_cast<cp::DampedSpring*>(constraint.get()))
    {
        spring->setAnchorA(cpvfliphor(spring->getAnchorA()));
        spring->setAnchorB(cpvfliphor(spring->getAnchorB()));
    }
    else if (auto spring = dynamic_cast<cp::DampedRotarySpring*>(constraint.get()))
        spring->setRestAngle(-spring->getRestAngle());
    else if (auto joint = dynamic_cast<cp::RotaryLimitJoint*>(constraint.get()))
    {
        joint->setMin(-joint->getMin());
        joint->setMax(-joint->getMax());
    }
    else if (auto ratchet = dynamic_cast<cp::RatchetJoint*>(constraint.get()))
    {
        ratchet->setPhase(-ratchet->getPhase());
        ratchet->setRatchet(-ratchet->getRatchet());
    }
    else if (auto gear = dynamic_cast<cp::GearJoint*>(constraint.get()))
    {
        gear->setPhase(-gear->getPhase());
        gear->setRatio(-gear->getRatio());
    }
    else if (auto motor = dynamic_cast<cp::SimpleMotor*>(constraint.get()))
        motor->setRate(-motor->getRate());
}
