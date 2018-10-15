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
#include "drawables/WrapPlane.hpp"
#include "resources/ResourceManager.hpp"


#include <string>

class GameScene;
class Renderer;

namespace background
{
    class Parallax final : public ::GameObject
    {
        WrapPlane plane;
        glm::vec2 internalDisplacement;
        float parallaxFactor;

    public:
        Parallax(GameScene& gameScene);
        Parallax(GameScene& gameScene, std::string textureName);
        virtual ~Parallax() {}

        virtual void update(FrameTime curTime);
        virtual void render(Renderer& renderer);
        virtual bool notifyScreenTransition(cpVect displacement);

        auto getParallaxFactor() { return parallaxFactor; }
        void setParallaxFactor(float factor) { parallaxFactor = factor; }

        struct ConfigStruct
        {
            glm::i16vec2 dummy;
            std::string textureName;
            float parallaxFactor;
        };

        bool configure(const ConfigStruct& config);
    };
}

bool readFromStream(InputStream& stream, background::Parallax::ConfigStruct& config);
