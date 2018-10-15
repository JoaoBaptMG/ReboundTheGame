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


#include <string>

#include "objects/InteractableObject.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/SegmentedSprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "gameplay/Script.hpp"

#include "language/LocalizationManager.hpp"

class GameScene;
class Renderer;

namespace props
{
    class MessageSign : public ::InteractableObject
    {
        Sprite signPole;
        SegmentedSprite signBox;
        TextDrawable signLabel;
        std::string messageString;

        LocalizationManager::CallbackEntry callbackEntry;

    public:
        MessageSign(GameScene& scene);
        virtual ~MessageSign() = default;

        virtual void interact() override;
        virtual void render(Renderer& renderer) override;

        struct ConfigStruct
        {
            glm::i16vec2 position;
            std::string messageString;
        };

        bool configure(const ConfigStruct& config);

        auto getDisplayPosition()
        {
            auto pos = interactionCenter;
            return glm::vec2(roundf(pos.x), roundf(pos.y+40));
        }
    };

    bool readFromStream(InputStream& stream, MessageSign::ConfigStruct& config);
}