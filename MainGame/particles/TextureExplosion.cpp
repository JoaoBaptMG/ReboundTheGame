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


#include "TextureExplosion.hpp"

#include <random>

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"

const TextureExplosion::DensityTag TextureExplosion::Density{};
const TextureExplosion::SizeTag TextureExplosion::Size{};

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    glm::vec2 acceleration, size_t depth) : GameObject(scene), texture(tex), drawingDepth(depth),
    vertices(sf::Quads), globalPosition(), globalAcceleration(acceleration), duration(duration), maleftOffset() {}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
    Duration duration, std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
    SizeTag, size_t width, size_t height, size_t depth) : TextureExplosion(scene, tex, duration, acceleration, depth)
{
    assignPieces(texRect, velocityFunction, TextureExplosion::Size, width, height);
}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
    Duration duration, std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
    DensityTag, size_t pieceSizeX, size_t pieceSizeY, size_t depth)
    : TextureExplosion(scene, tex, texRect, duration, velocityFunction, acceleration, TextureExplosion::Size,
        tex->getSize().x/pieceSizeX, tex->getSize().y/pieceSizeY, depth) {}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
    SizeTag, size_t width, size_t height, size_t depth) : TextureExplosion(scene, tex, duration, acceleration, depth)
{
    assignPieces(velocityFunction, TextureExplosion::Size, width, height);
}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    std::function<glm::vec2(float,float)> velocityFunction, glm::vec2 acceleration,
    DensityTag, size_t pieceSizeX, size_t pieceSizeY, size_t depth)
    : TextureExplosion(scene, tex, duration, velocityFunction, acceleration, TextureExplosion::Size,
        tex->getSize().x/pieceSizeX, tex->getSize().y/pieceSizeY, depth) {}

inline static auto randomGenerator(util::rect velocityRect)
{
    std::random_device init;
	std::mt19937 rgen(init());
	std::uniform_real_distribution<float> distribution;
	auto generator = std::bind(distribution, rgen);

    return [velocityRect, generator](float x, float y) mutable
    {
        float xVel = velocityRect.x + generator() * velocityRect.width;
        float yVel = velocityRect.y + generator() * velocityRect.height;
        return glm::vec2(xVel, yVel);
    };
}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
    Duration duration, util::rect velocityRect, glm::vec2 acceleration, SizeTag, size_t width, size_t height,
    size_t depth) : TextureExplosion(scene, tex, duration, acceleration, depth)
{
    assignPieces(texRect, randomGenerator(velocityRect), TextureExplosion::Size, width, height);
}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, util::rect texRect,
    Duration duration, util::rect velocityRect, glm::vec2 acceleration, DensityTag, size_t pieceSizeX,
    size_t pieceSizeY, size_t depth)
    : TextureExplosion(scene, tex, texRect, duration, velocityRect, acceleration, TextureExplosion::Size,
        texRect.width/pieceSizeX, texRect.height/pieceSizeY, depth) {}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    util::rect velocityRect, glm::vec2 acceleration, SizeTag, size_t width, size_t height, size_t depth)
    : TextureExplosion(scene, tex, duration, acceleration, depth)
{
    assignPieces(randomGenerator(velocityRect), TextureExplosion::Size, width, height);
}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    util::rect velocityRect, glm::vec2 acceleration, DensityTag, size_t pieceSizeX, size_t pieceSizeY,
    size_t depth)
    : TextureExplosion(scene, tex, duration, velocityRect, acceleration, TextureExplosion::Size,
        tex->getSize().x/pieceSizeX, tex->getSize().y/pieceSizeY, depth) {}

sf::Vector2f toSF(glm::vec2 v);

void TextureExplosion::assignPieces(util::rect texRect,
    std::function<glm::vec2(float,float)> velocityFunction, SizeTag, size_t width, size_t height)
{
    this->texRect = texRect;
    this->width = width, this->height = height;
    texturePieces.resize(width*height);
    vertices.resize(4*width*height);

    float pieceWidth = (float)texRect.width/width;
    float pieceHeight = (float)texRect.height/height;

    glm::vec2 offset{texRect.x, texRect.y};
    for (size_t j = 0; j < height; j++)
        for (size_t i = 0; i < width; i++)
        {
            float px = (i + 0.5)/width - 0.5;
            float py = (j + 0.5)/height - 0.5;

            texturePieces[j*width+i].position.x = texRect.width * px;
            texturePieces[j*width+i].position.y = texRect.height * py;
            texturePieces[j*width+i].velocity = velocityFunction(2*px, 2*py);

            vertices[4*(j*width+i)].texCoords = toSF(offset + glm::vec2(pieceWidth*i, pieceHeight*j));
            vertices[4*(j*width+i)+1].texCoords = toSF(offset + glm::vec2(pieceWidth*(i+1), pieceHeight*j));
            vertices[4*(j*width+i)+2].texCoords = toSF(offset + glm::vec2(pieceWidth*(i+1), pieceHeight*(j+1)));
            vertices[4*(j*width+i)+3].texCoords = toSF(offset + glm::vec2(pieceWidth*i, pieceHeight*(j+1)));

            for (size_t k = 0; k < 4; k++) vertices[4*(j*width+i)].color = sf::Color::White;
        }
}

void TextureExplosion::assignPieces(util::rect texRect,
    std::function<glm::vec2(float,float)> velocityFunction, DensityTag, size_t pieceSizeX, size_t pieceSizeY)
{
    assignPieces(texRect, velocityFunction, TextureExplosion::Size,
        texRect.width/pieceSizeX, texRect.height/pieceSizeY);
}

void TextureExplosion::assignPieces(std::function<glm::vec2(float,float)> velocityFunction,
    SizeTag, size_t width, size_t height)
{
    util::rect texRect{0, 0, (float)texture->getSize().x, (float)texture->getSize().y};
    assignPieces(texRect, velocityFunction, TextureExplosion::Size, width, height);
}

void TextureExplosion::assignPieces(std::function<glm::vec2(float,float)> velocityFunction,
    DensityTag, size_t pieceSizeX, size_t pieceSizeY)
{
    util::rect texRect{0, 0, (float)texture->getSize().x, (float)texture->getSize().y};
    assignPieces(texRect, velocityFunction, TextureExplosion::Density, pieceSizeX, pieceSizeY);
}

void TextureExplosion::setOffsetFunction(std::function<TextureExplosion::Duration(float,float)> func)
{
    timeOffsets.resize(width*height);

    for (size_t j = 0; j < height; j++)
        for (size_t i = 0; i < width; i++)
        {
            timeOffsets[j*width+i] = func(2*((i + 0.5)/width - 0.5), 2*((j + 0.5)/height - 0.5));
            maleftOffset = std::max(maleftOffset, timeOffsets[j*width+i]);
        }
}

void TextureExplosion::resetOffsets() { timeOffsets.clear(); maleftOffset = Duration(); }

void TextureExplosion::update(FrameTime curTime)
{
    if (lastTime == decltype(lastTime)()) lastTime = curTime;
    if (initialTime == decltype(initialTime)()) initialTime = curTime;
    this->curTime = curTime;

    auto totalDuration = curTime - initialTime;
    if (totalDuration > duration + maleftOffset) remove();
    auto dt = toSeconds<float>(curTime - lastTime);

    size_t i = 0;
    for (auto& data : texturePieces)
    {
        if (timeOffsets.empty() || totalDuration > timeOffsets[i])
        {
            data.position += data.velocity * dt;
            data.velocity += globalAcceleration * dt;
        }

        i++;
    }

    lastTime = curTime;
}

void TextureExplosion::render(Renderer& renderer)
{
    float opacity = 1.0f - toSeconds<float>(curTime - initialTime) / toSeconds<float>(duration);

    float pieceWidth = (float)texRect.width/width;
    float pieceHeight = (float)texRect.height/height;
    
    for (size_t i = 0; i < texturePieces.size(); i++)
    {
        float localOpacity = opacity;
        if (!timeOffsets.empty())
            localOpacity += toSeconds<float>(timeOffsets[i]) / toSeconds<float>(duration);

        if (localOpacity > 1.0f) localOpacity = 1.0f;
        if (localOpacity < 0.0f) localOpacity = 0.0f;

        for (size_t k = 0; k < 4; k++)
        {
            vertices[4*i+k].color.a = localOpacity * 255.0f;
            vertices[4*i+k].position = toSF(globalPosition + texturePieces[i].position);
        }

        vertices[4*i].position += sf::Vector2f(-0.5 * pieceWidth, -0.5 * pieceHeight);
        vertices[4*i+1].position += sf::Vector2f(+0.5 * pieceWidth, -0.5 * pieceHeight);
        vertices[4*i+2].position += sf::Vector2f(+0.5 * pieceWidth, +0.5 * pieceHeight);
        vertices[4*i+3].position += sf::Vector2f(-0.5 * pieceWidth, +0.5 * pieceHeight);
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;
    states.texture = texture.get();
    renderer.pushDrawable(vertices, states, drawingDepth);
}
