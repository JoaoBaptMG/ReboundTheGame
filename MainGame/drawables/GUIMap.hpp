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

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <chronoUtils.hpp>

class LevelData;

class GUIMap final : public sf::Drawable
{
    std::shared_ptr<LevelData> curLevel;
    sf::Texture* mapTexture;
    sf::VertexArray vertArray;
    sf::Vector2f displayPosition;
    sf::Color mapBlinkColor;
    size_t curRoom;

    FrameTime initTime;
    bool extendedFrame;
    
public:
    GUIMap(bool extendedFrame = false) : extendedFrame(extendedFrame) {}
    ~GUIMap() {}
    
    void update(FrameTime curTime);
    
    auto getCurLevel() const { return curLevel; }
    void setCurLevel(std::shared_ptr<LevelData> level);
    void buildLevelTexture();
    
    auto getCurRoom() const { return curRoom; }
    void setCurRoom(size_t room) { curRoom = room; }
    
    auto getDisplayPosition() const { return displayPosition; }
    void setDisplayPosition(sf::Vector2f pos) { displayPosition = pos; }
    
    auto getExtendedFrame() const { return extendedFrame; }
    void setExtendedFrame(bool f) { extendedFrame = f; }

    sf::FloatRect getBounds() const;
    
    void presentRoom(size_t room);
    void presentRoomFull(size_t room);
    void hideRoom(size_t room);
    
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
