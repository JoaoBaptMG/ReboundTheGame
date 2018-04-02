#include "LevelTransition.hpp"

#include <chronoUtils.hpp>
#include "defaults.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"

constexpr auto TransitionDuration = 60 * UpdatePeriod;

LevelTransition::LevelTransition(GameScene &scene) : gameScene(scene), curTime(), transitionTime(),
    screenQuad(sf::FloatRect(0, 0, ScreenWidth, ScreenHeight), sf::Color::Black), transitionDone(false)
{
    
}

void LevelTransition::requestLevelTransition(std::string name)
{
    transitionTime = curTime;
    nextLevel = name;
    transitionDone = false;
}

void LevelTransition::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    if (!isNull(transitionTime))
    {
        if (!transitionDone && curTime > transitionTime + TransitionDuration/2)
        {
            transitionDone = true;
            
            if (nextLevel.empty()) gameScene.reloadLevel();
            else gameScene.loadLevel(nextLevel);
        }
        if (curTime > transitionTime + TransitionDuration)
            reset(transitionTime);
    }
}

void LevelTransition::render(Renderer& renderer)
{
    if (!isNull(transitionTime))
    {
        float factor = 0;
        if (curTime > transitionTime + TransitionDuration) return;
        
        if (curTime > transitionTime + TransitionDuration/2)
            factor = 2.0f - toSeconds<float>(curTime - transitionTime) / toSeconds<float>(TransitionDuration/2);
        else factor = toSeconds<float>(curTime - transitionTime) / toSeconds<float>(TransitionDuration/2);
        
        screenQuad.setAlpha(255 * factor);
        
        renderer.pushDrawable(screenQuad, {}, 10000000);
    }
}
