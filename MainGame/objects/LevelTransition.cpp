//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "LevelTransition.hpp"

#include <chronoUtils.hpp>
#include "defaults.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"

constexpr auto TransitionDuration = 60_frames;

LevelTransition::LevelTransition(GameScene &scene) : gameScene(scene), curTime(), transitionTime(),
    screenQuad(util::rect(0, 0, ScreenWidth, ScreenHeight), sf::Color::Black), transitionDone(false)
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
