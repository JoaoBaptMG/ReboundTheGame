#pragma once

#include "EnemyCommon.hpp"
#include "drawables/Sprite.hpp"
#include <SFML/System.hpp>

#include <memory>
#include <cppmunk/Body.h>
#include <cppmunk/Shape.h>
#include <cppmunk/Constraint.h>

class GameScene;
class Renderer;

namespace enemies
{
    class Hopper final : public EnemyCommon
    {
        Sprite hopperBody, hopperLeg, hopperFoot;
        
        InteractionHandler bombShapeHandler, dashShapeHandler;
        std::shared_ptr<cp::Body> mainBody, footBody;
        std::shared_ptr<cp::Shape> mainShape[2], footShape, damageShape;
        std::shared_ptr<cp::Constraint> launcherSpring;
        
        cpVect graphicalDisplacement;
        bool facingRight;
        
        void setupPhysics(cpVect position);
        void setFacingDirection(bool facingRight);
        
    public:
        Hopper(GameScene& gameScene);
        virtual ~Hopper();
        
        virtual size_t playerDamage(Player& player, const std::shared_ptr<cp::Shape> shape) const override;
        virtual void die() override;
        
        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
    
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            bool facingRight;
        };
        
        bool configure(const ConfigStruct& config);
    };
    
    bool readFromStream(sf::InputStream& stream, Hopper::ConfigStruct& config);
}
