#pragma once

#include "drawables/Sprite.hpp"
#include "drawables/GUIMeter.hpp"
#include "drawables/TextDrawable.hpp"
#include "objects/Player.hpp"
#include <chronoUtils.hpp>
#include "drawables/GUIMap.hpp"

class GameScene;
class Renderer;

class GUI final
{
    Sprite guiLeft, guiRight;
    GUIMeter playerMeter, dashMeter;
    Sprite bombSprites[MaxBombs];
    TextDrawable levelLabel, levelID;
    size_t levelNumber;
    bool drawDash;

    GUIMap guiMap;

    std::string lastIconName;
    GameScene& gameScene;
    
    void configureText();
    
    std::chrono::steady_clock::time_point lastTime;
    float healthBlinkPhase;
    
public:
    GUI(GameScene& scene);
    ~GUI() {}

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);

    void setLevelNumber(size_t number);
    void setVisibleMaps(const std::vector<bool>& visibleMaps);
};
