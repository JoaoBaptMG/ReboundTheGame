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
        
        virtual void update(FrameTime curTime) override;
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
