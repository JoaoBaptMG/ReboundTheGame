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

#include "PlayerDeath.hpp"

#include "Player.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"
#include "defaults.hpp"
#include <chronoUtils.hpp>

#include "ColorList.hpp"

#include "particles/ParticleBatch.hpp"
#include "scene/GameScene.hpp"

constexpr auto FadeDuration = 30_frames;
constexpr auto TotalDuration = 200_frames;

PlayerDeath::PlayerDeath(GameScene& scene, Player& player, const std::shared_ptr<Texture>& texture)
    : GameObject(scene), playerSprite(texture), position(player.getDisplayPosition()), spawnedParticle(false)
{
    
}

void PlayerDeath::update(FrameTime curTime)
{
    if (initTime == decltype(initTime)()) initTime = curTime;
    
    auto duration = curTime - initTime;
    
    if (duration <= FadeDuration)
    {
        float factor = toSeconds<float>(duration) / toSeconds<float>(FadeDuration);
        playerSprite.setFlashColor(glm::u8vec4(255, 255, 255, 255 * factor));
    }
    else if (duration <= 2 * FadeDuration)
    {
        if (!spawnedParticle)
        {
            auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "player-death", (size_t)32);
            batch->setPosition(position);
            gameScene.addObject(std::move(batch));
            
            spawnedParticle = true;
        }
        
        float factor = toSeconds<float>(duration - FadeDuration) / toSeconds<float>(FadeDuration);
        playerSprite.setOpacity(1 - factor);
    }
    else playerSprite.setFlashColor(Colors::White);
    
    if (duration > TotalDuration)
    {
        gameScene.requestGameoverScene();
        remove();
    }
}

void PlayerDeath::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(position);
    renderer.pushDrawable(playerSprite, 25);
    renderer.popTransform();
}
