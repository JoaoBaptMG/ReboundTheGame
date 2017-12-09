#pragma once

#include "drawables/Sprite.hpp"
#include "drawables/GUIMeter.hpp"
#include "objects/Player.hpp"
#include <chronoUtils.hpp>

class GameScene;
class Renderer;

class GUI final
{
    Sprite guiLeft, guiRight;
    GUIMeter playerMeter, dashMeter;
    std::vector<Sprite> abilitySprites;
    Sprite bombSprites[MaxBombs];
    bool drawDash;

    std::string lastIconName;
    GameScene& gameScene;
    
public:
    GUI(GameScene& scene);
    ~GUI() {}

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
