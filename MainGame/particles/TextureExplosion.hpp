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


#include <vector>
#include <chronoUtils.hpp>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <rect.hpp>

#include "objects/GameObject.hpp"

class GameScene;
class Renderer;

class TextureExplosion final : public GameObject
{
public:
    using TimePoint = FrameTime;
    using Duration = TimePoint::duration;

    struct TexPiece
    {
        glm::vec2 position, velocity;
    };

private:
    std::shared_ptr<sf::Texture> texture;
    std::vector<TexPiece> texturePieces;
    std::vector<Duration> timeOffsets;
    sf::VertexArray vertices;
    glm::vec2 globalPosition, globalAcceleration;
    size_t width, height, drawingDepth;
    TimePoint curTime, lastTime, initialTime;
    Duration duration, maleftOffset;
    util::rect texRect;
    
public:
    const static struct DensityTag {} Density;
    const static struct SizeTag {} Size;

    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
         glm::vec2 acceleration, size_t depth = 12);
    void assignPieces(util::rect texRect, std::function<glm::vec2(float,float)> velocityFunction,
        SizeTag, size_t width, size_t height);
    void assignPieces(util::rect texRect, std::function<glm::vec2(float,float)> velocityFunction,
        DensityTag, size_t pieceSizeX, size_t pieceSizeY);
    void assignPieces(std::function<glm::vec2(float,float)> velocityFunction,
        SizeTag, size_t width, size_t height);
    void assignPieces(std::function<glm::vec2(float,float)> velocityFunction,
        DensityTag, size_t pieceSizeX, size_t pieceSizeY);

    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
        Duration duration, std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
        SizeTag, size_t width, size_t height, size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
        Duration duration, std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
        DensityTag, size_t pieceSizeX, size_t pieceSizeY, size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
        std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
        SizeTag, size_t width, size_t height, size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
        std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
        DensityTag, size_t pieceSizeX, size_t pieceSizeY, size_t depth = 12);
    
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
        Duration duration, util::rect velocityRect, glm::vec2 acceleration,
        SizeTag, size_t width, size_t height, size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
        Duration duration, util::rect velocityRect, glm::vec2 acceleration,
        DensityTag, size_t pieceSizeX, size_t pieceSizeY, size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
        util::rect velocityRect, glm::vec2 acceleration, SizeTag, size_t width, size_t height,
        size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
        util::rect velocityRect, glm::vec2 acceleration, DensityTag, size_t pieceSizeX, size_t pieceSizeY,
        size_t depth = 12);

    auto getPosition() { return globalPosition; }
    void setPosition(glm::vec2 pos) { globalPosition = pos; }

    void setOffsetFunction(std::function<Duration(float,float)> func);
    void resetOffsets();

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;
};
