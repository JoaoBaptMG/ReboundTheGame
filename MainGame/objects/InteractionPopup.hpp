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

#include "GameObject.hpp"

#include "drawables/SegmentedSprite.hpp"
#include "drawables/TextDrawable.hpp"

#include <memory>
#include <chronoUtils.hpp>


class GameScene;
class Renderer;

class InteractionPopup final : public GameObject
{
    SegmentedSprite popupBox;
    TextDrawable popupLabel;

    FrameTime curTime, destTime;

    glm::vec2 position;
    bool collapsing;

public:
    explicit InteractionPopup(GameScene& scene);
    virtual ~InteractionPopup() = default;

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;

    virtual bool notifyScreenTransition(cpVect displacement) override;

    auto getPosition() const { return position; }
    void setPosition(glm::vec2 pos) { position = pos; }

    void collapse();
};