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

#include <memory>

#include <chipmunk/chipmunk.h>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"
#include "drawables/Tilemap.hpp"

class GameScene;
class Renderer;

namespace props
{
    class PushableCrate final : public ::GameObject
    {
        static void setupCollisionHandlers(cp::Space* space);

        Tilemap tilemap;
        std::shared_ptr<cp::Shape> shape;
        size_t belongingRoomID;

    public:
        PushableCrate(GameScene& scene);
        virtual ~PushableCrate();

        void setupPhysics(float width, float height);

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;

        auto getPosition() const { return shape->getBody()->getPosition(); }
        void setPosition(cpVect pos) { shape->getBody()->setPosition(pos); }

        auto getDisplayPosition() const
        {
            auto vec = getPosition();
            return glm::vec2((float)std::round(vec.x), (float)std::round(vec.y));
        }

        auto getPositionKey() const
        {
            return std::to_string(belongingRoomID) + "." + getName() + ".position";
        }
        
        auto getRotationKey() const
        {
            return std::to_string(belongingRoomID) + "." + getName() + ".rotation";
        }

#pragma pack(push, 1)
        struct ConfigStruct
        {
            glm::i16vec2 position;
            int16_t width, height;
        };

        static_assert(sizeof(ConfigStruct) == 8*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);

        static constexpr cpCollisionType CollisionType = 'cpsh';
    };
}
