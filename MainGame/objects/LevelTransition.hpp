#pragma once

#include <chrono>
#include "drawables/Quad.hpp"

class Renderer;
class GameScene;

class LevelTransition final
{
    std::chrono::steady_clock::time_point curTime, transitionTime;
    Quad screenQuad;
    std::string nextLevel;
    bool transitionDone;
    GameScene& gameScene;
    
public:
    LevelTransition(GameScene& scene);
    ~LevelTransition() {}
    
    void requestLevelTransition(std::string level);
    
    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
