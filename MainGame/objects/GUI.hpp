#pragma once

#include "drawables/Sprite.hpp"
#include "drawables/GUIMeter.hpp"
#include <chronoUtils.hpp>

class GameScene;
class Renderer;

class GUI
{
    Sprite guiLeft, guiRight;
    GUIMeter playerMeter;

    GameScene& gameScene;
    
public:
    GUI(GameScene& scene);
    ~GUI() {}

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
