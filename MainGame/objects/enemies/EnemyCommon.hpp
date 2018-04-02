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
