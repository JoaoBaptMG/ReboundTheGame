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


#pragma once

#include "objects/Enemy.hpp"
#include "drawables/Sprite.hpp"

#include <chronoUtils.hpp>

class GameScene;
class Renderer;

namespace enemies
{
    class EnemyCommon : public ::Enemy
    {
        size_t health, touchDamage, dashDamage, bombDamage;
        FrameTime blinkTime;
        
    protected:
        FrameTime curTime;
        
        void addDashShape(std::shared_ptr<cp::Shape> shape, InteractionHandler& handler);
        void addBombShape(std::shared_ptr<cp::Shape> shape, InteractionHandler& handler);
        
        auto getTouchDamage() const { return touchDamage; }
        void setTouchDamage(size_t dam) { touchDamage = dam; }
        
        auto getDashDamage() const { return dashDamage; }
        void setDashDamage(size_t dam) { dashDamage = dam; }
        
        auto getBombDamage() const { return bombDamage; }
        void setBombDamage(size_t dam) { bombDamage = dam; }
        
        void applyBlinkEffect(Sprite& sprite);
        
        EnemyCommon(GameScene& scene);
        
    public:
        virtual ~EnemyCommon();
        
        virtual bool onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape) override;
        virtual bool onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape) override;
        
        virtual size_t playerDamage(Player& player, const std::shared_ptr<cp::Shape> shape) const = 0;
        virtual void damage(size_t amount);
        virtual void die();
        
        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;
        
        auto getHealth() const { return health; }
        void setHealth(size_t h) { health = h; }
    };
}
