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


#include "Camera.hpp"

#include "scene/GameScene.hpp"
#include <defaults.hpp>
#include <chronoUtils.hpp>
#include <cmath>
#include <random>
#include <functional>
#include <algorithm>

constexpr auto TransitionDuration = 20_frames;

void Camera::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    auto player = gameScene.getObjectByName<Player>("player");
    if (player)
    {
        position = player->getDisplayPosition();
        
        if (position.x < PlayfieldWidth/2) position.x = PlayfieldWidth/2;
        else if (position.x > (float)DefaultTileSize * gameScene.getRoomWidth() - PlayfieldWidth/2)
            position.x = (float)DefaultTileSize * gameScene.getRoomWidth() - PlayfieldWidth/2;
            
        if (position.y < PlayfieldHeight/2) position.y = PlayfieldHeight/2;
        else if (position.y > (float)DefaultTileSize * gameScene.getRoomHeight() - PlayfieldHeight/2)
            position.y = (float)DefaultTileSize * gameScene.getRoomHeight() - PlayfieldHeight/2;
    }
    
    if (!isNull(transitionTime) && curTime > transitionTime + TransitionDuration)
    {
        reset(transitionTime);
        gameScene.notifyTransitionEnded();
    }
}

void Camera::scheduleTransition(Camera::Direction direction)
{
    transitionDirection = direction;
    if (isNull(transitionTime)) transitionTime = curTime;
    else transitionTime = curTime + (curTime - transitionTime) - TransitionDuration;
}

bool Camera::transitionOccuring() const
{
    return !isNull(transitionTime);
}

void Camera::applyShake(FrameDuration duration,
    FrameDuration period, float amp)
{
    shakePeriod = period;
    shakeTime = curTime + duration;
    shakeSamples.resize(duration / shakePeriod + 2);

    std::random_device init;
	std::mt19937 rgen(init());
	std::uniform_real_distribution<float> distribution(-amp, amp);
	auto generator = std::bind(distribution, rgen);
    auto generator2 = [&] { return sf::Vector2f(generator(), generator()); };

    std::generate(shakeSamples.begin(), shakeSamples.end(), generator2);
    for (size_t i = 0; i < shakeSamples.size(); i++)
    {
        float t = (float)i / shakeSamples.size();
        shakeSamples[i] *= t*t;
    }
}

sf::Vector2f Camera::getGlobalDisplacement() const
{
    auto disp = position;
    
    if (!isNull(transitionTime))
    {
        float factor = toSeconds<float>(curTime - transitionTime) / toSeconds<float>(TransitionDuration) - 0.5;
        
        switch (transitionDirection)
        {
            case Direction::Right: disp.x = factor * PlayfieldWidth; break;
            case Direction::Down: disp.y = factor * PlayfieldHeight; break;
            case Direction::Left:
                disp.x = floorf((float)DefaultTileSize * gameScene.getRoomWidth() - factor * PlayfieldWidth);
                break;
            case Direction::Up:
                disp.y = floorf((float)DefaultTileSize * gameScene.getRoomHeight() - factor * PlayfieldHeight);
                break;
        }
    }
    
    return sf::Vector2f(ScreenWidth, ScreenHeight)/2.0f - disp + getShakeDisplacement();
}

sf::Vector2f Camera::getShakeDisplacement() const
{
    if (isNull(shakeTime)) return sf::Vector2f(0, 0);
    if (curTime > shakeTime) return sf::Vector2f(0, 0);

    auto sample = (shakeTime - curTime) / shakePeriod;
    auto rem = (shakeTime - curTime) % shakePeriod;
    float factor = toSeconds<float>(rem) / toSeconds<float>(shakePeriod);

    auto s1 = shakeSamples.at(sample), s2 = shakeSamples.at(sample+1);
    return s1 + factor * (s2 - s1);
}

bool Camera::isVisible(sf::Vector2f point) const
{
    return fabsf(point.x - position.x) <= PlayfieldWidth/2 && fabsf(point.y - position.y) <= PlayfieldHeight/2;
}

bool Camera::isVisible(sf::FloatRect rect) const
{
    sf::FloatRect centerRect(position.x - PlayfieldWidth/2, position.y - PlayfieldHeight/2, PlayfieldWidth, PlayfieldHeight);
    return rect.intersects(centerRect);
}
