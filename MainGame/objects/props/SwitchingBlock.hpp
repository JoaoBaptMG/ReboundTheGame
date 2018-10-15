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

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"

#include "drawables/Sprite.hpp"

#include <vector>
#include <chronoUtils.hpp>

#include <cppmunk/PolyShape.h>

class GameScene;
class Renderer;

namespace props
{
    class SwitchingBlockCluster;

    class SwitchingBlock final : public ::GameObject
    {
        Sprite blockSprite, fadeSprite;
        std::shared_ptr<cp::Shape> blockShape;

        bool visible;
        std::string blockClusterName;
        size_t blockTime;
        SwitchingBlockCluster* parentBlockCluster;

        FrameTime fadeTime, curTime;

        void setupPhysics();

    public:
        SwitchingBlock(GameScene& scene);
        virtual ~SwitchingBlock();

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;

        virtual bool notifyScreenTransition(cpVect displacement) override;

        void switchOn();
        void switchOff();
        void doFade();

        static FrameDuration getFadeDuration();

        auto getPosition() const { return blockShape->getBody()->getPosition(); }
        void setPosition(cpVect pos) { blockShape->getBody()->setPosition(pos); }

        auto getDisplayPosition() const
        {
            auto vec = getPosition();
            return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
        }

        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            std::string blockClusterName;
            size_t blockTime;
        };

        bool configure(const ConfigStruct& config);
    };

    bool readFromStream(sf::InputStream& stream, SwitchingBlock::ConfigStruct& config);
}