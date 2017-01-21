#pragma once
#include <map>
#include <list>
#include <functional>
#include "../utility/non_copyable_movable.hpp"

class Renderer;

class Scene : util::non_copyable_movable
{
public:
    Scene() {}
    virtual ~Scene() {}

    virtual void update(float dt) = 0;
    virtual void render(Renderer &renderer) = 0;
};

