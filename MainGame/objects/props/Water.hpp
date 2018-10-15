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
#include "objects/Player.hpp"

#include "drawables/WaterBody.hpp"


#include <chipmunk/chipmunk.h>
#include <chronoUtils.hpp>

class GameScene;
class Renderer;

namespace props
{
    class Water final : public GameObject
    {
        cpBB rect;
        WaterBody shape;

        cpFloat oldArea;

    public:
        Water(GameScene& scene);
        virtual ~Water();

        void setRect(util::irect rect);

        auto getPosition() { return cpVect{rect.l, rect.b}; }
        auto getDisplayPosition()
        {
            auto pos = getPosition();
            return glm::vec2((float)std::round(pos.x), (float)std::round(pos.y));
        }
        
        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;
        
    #pragma pack(push, 1)
        struct ConfigStruct
        {
            glm::i16vec2 position;
            int16_t width, height;
            uint8_t hideTop;
            glm::u8vec4 coastColor, color;
        };

        static_assert(sizeof(ConfigStruct) == 17*sizeof(char), "Packing failed!");
    #pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
