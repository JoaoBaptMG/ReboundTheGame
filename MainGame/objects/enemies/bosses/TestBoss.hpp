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

#include "objects/enemies/Boss.hpp"
#include <chronoUtils.hpp>

class GameScene;
class Renderer;

namespace enemies
{
    namespace bosses
    {
        class TestBoss : public enemies::Boss
        {
            Sprite mainSprite;
            std::shared_ptr<cp::Shape> collisionShapes[3];
            InteractionHandler bombShapeHandler;
            cpFloat leftSpan, rightSpan;
            FrameTime initTime;
            
            void setupPhysics();
            
        public:
            TestBoss(GameScene& scene);
            virtual ~TestBoss();
            
            virtual size_t playerDamage(Player& player, const std::shared_ptr<cp::Shape> shape) const override;
            virtual void die() override;
            
            virtual void update(FrameTime curTime) override;
            virtual void render(Renderer& renderer) override;
            
#pragma pack(push, 1)
            struct ConfigStruct
            {
                sf::Vector2<int16_t> position;
                int16_t leftSpan, rightSpan;
            };
            
            static_assert(sizeof(ConfigStruct) == 8*sizeof(char), "Packing failed!");
#pragma pack(pop)
            
            bool configure(const ConfigStruct& config);
            
            virtual size_t getMaxHealth() const override;
        };
        
        class TestBossProjectile : public ::Enemy
        {
            Sprite sprite;
            std::shared_ptr<cp::Shape> collisionShape;
            
            void setupPhysics();
            
        public:
            TestBossProjectile(GameScene& scene, cpVect pos, cpVect vel);
            virtual ~TestBossProjectile();
            
            virtual bool onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape) override;
            virtual bool onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape) override { return true; }
            
            virtual void update(FrameTime curTime) override;
            virtual void render(Renderer& renderer) override;
        };
        
        class TimedLevelWarper : public ::GameObject
        {
            std::string levelName;
            FrameTime initTime;
            
        public:
            TimedLevelWarper(GameScene& scene, std::string level) : GameObject(scene), levelName(level) {}
            virtual ~TimedLevelWarper() {}
            
            virtual void update(FrameTime curTime) override;
            virtual void render(Renderer& renderer) override {}
        };
    }
}
