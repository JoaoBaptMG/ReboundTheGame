#pragma once

#include <chronoUtils.hpp>
#include "drawables/Quad.hpp"

class Renderer;
class GameScene;

class LevelTransition final
{
    FrameTime curTime, transitionTime;
    Quad screenQuad;
    std::string nextLevel;
    bool transitionDone;
    GameScene& gameScene;
    
public:
    LevelTransition(GameScene& scene);
    ~LevelTransition() {}
    
    void requestLevelTransition(std::string level);
    
    void update(FrameTime curTime);
    void render(Renderer& renderer);
};
