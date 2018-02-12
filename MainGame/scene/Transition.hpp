#pragma once

#include "Scene.hpp"
#include "drawables/Quad.hpp"
#include <memory>

class Transition final : public Scene
{
    Scene *prevScene, *nextScene;
    std::chrono::steady_clock::time_point transitionBegin, transitionEnd;
    float transitionFactor; bool releasePrev, isPop;
    Quad transitionQuad;
    
public:
    Transition(Scene* prevScene, Scene* nextScene, decltype(transitionBegin) transitionBegin,
        decltype(transitionBegin) transitionEnd, bool releasePrev = false, bool isPop = false);
    virtual ~Transition() { if (!releasePrev) delete prevScene; }
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer &renderer) override;
};
