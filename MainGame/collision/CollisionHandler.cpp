#include "CollisionHandler.hpp"
#include <functional>

CollisionHandler::CollisionHandler() : curSpace(cp::SpaceMake())
{
    cpSpaceSetGravity(curSpace.get(), { 0, 900 });
}

CollisionHandler::~CollisionHandler()
{
}

void CollisionHandler::update(float dt)
{
    cpSpaceStep(curSpace.get(), dt);
}

cp::CollisionBodyHandle CollisionHandler::newDynamicBody()
{
    return cp::CollisionBodyHandle(curSpace.get(), cp::BodyMake(0, 0));
}

cp::CollisionBodyHandle CollisionHandler::newKinematicBody()
{
    return cp::CollisionBodyHandle(curSpace.get(), cp::BodyMakeKinematic());
}

cp::CollisionBodyHandle CollisionHandler::newStaticBody()
{
    return cp::CollisionBodyHandle(curSpace.get(), cp::BodyMakeStatic());
}
