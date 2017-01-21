#pragma once

#include "../utility/non_copyable_movable.hpp"
#include "chipmunkWrapperTypes.hpp"
#include "CollisionBodyHandle.hpp"
#include <list>

enum class CollisionGroup { Player, Level, NumberOfCollisionGroups };

class CollisionHandler final : util::non_copyable_movable
{
    cp::SpacePtr curSpace;

public:
    CollisionHandler();
    ~CollisionHandler();

    void update(float dt);

    cp::CollisionBodyHandle newDynamicBody();
    cp::CollisionBodyHandle newKinematicBody();
    cp::CollisionBodyHandle newStaticBody();
};

