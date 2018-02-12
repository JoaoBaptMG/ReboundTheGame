#pragma once

#include "objects/enemies/Boss.hpp"
#include <chrono>

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
            std::chrono::steady_clock::time_point initTime;
            
            void setupPhysics();
            
        public:
            TestBoss(GameScene& scene);
            virtual ~TestBoss();
            
            virtual size_t playerDamage(Player& player, const std::shared_ptr<cp::Shape> shape) const override;
            virtual void die() override;
            
            virtual void update(std::chrono::steady_clock::time_point curTime) override;
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
            
            virtual void update(std::chrono::steady_clock::time_point curTime) override;
            virtual void render(Renderer& renderer) override;
        };
        
        class TimedLevelWarper : public ::GameObject
        {
            std::string levelName;
            std::chrono::steady_clock::time_point initTime;
            
        public:
            TimedLevelWarper(GameScene& scene, std::string level) : GameObject(scene), levelName(level) {}
            virtual ~TimedLevelWarper() {}
            
            virtual void update(std::chrono::steady_clock::time_point curTime) override;
            virtual void render(Renderer& renderer) override {}
        };
    }
}
