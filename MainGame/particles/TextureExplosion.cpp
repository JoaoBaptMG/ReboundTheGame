#include "TextureExplosion.hpp"

#include <random>

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"

const TextureExplosion::DensityTag TextureExplosion::Density{};
const TextureExplosion::SizeTag TextureExplosion::Size{};

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    sf::FloatRect velocityRect, sf::Vector2f acceleration, SizeTag, size_t width, size_t height, size_t depth)
    : GameObject(scene), texture(tex), texturePieces(width*height), vertices(sf::Quads, 4*width*height),
    width(width), height(height), drawingDepth(depth), globalPosition(), globalAcceleration(acceleration),
    duration(duration)
{
	std::random_device init;
	std::mt19937 rgen(init());
	std::uniform_real_distribution<float> distribution;
	auto generator = std::bind(distribution, rgen);
    
    auto texSize = tex->getSize();
    float pieceWidth = (float)texSize.x/width;
    float pieceHeight = (float)texSize.y/height;
    
    for (size_t j = 0; j < height; j++)
        for (size_t i = 0; i < width; i++)
        {
            texturePieces[j*width+i].position.x = texSize.x * ((i + 0.5)/width - 0.5);
            texturePieces[j*width+i].position.y = texSize.y * ((j + 0.5)/height - 0.5);

            float xVel = velocityRect.left + generator() * velocityRect.width;
            float yVel = velocityRect.top + generator() * velocityRect.height;
            texturePieces[j*width+i].velocity = sf::Vector2f(xVel, yVel);

            vertices[4*(j*width+i)].texCoords = sf::Vector2f(pieceWidth*i, pieceHeight*j);
            vertices[4*(j*width+i)+1].texCoords = sf::Vector2f(pieceWidth*(i+1), pieceHeight*j);
            vertices[4*(j*width+i)+2].texCoords = sf::Vector2f(pieceWidth*(i+1), pieceHeight*(j+1));
            vertices[4*(j*width+i)+3].texCoords = sf::Vector2f(pieceWidth*i, pieceHeight*(j+1));

            for (size_t k = 0; k < 4; k++) vertices[4*(j*width+i)].color = sf::Color::White;
        }
}

TextureExplosion::TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
    sf::FloatRect velocityRect, sf::Vector2f acceleration, DensityTag, size_t pieceSizeX, size_t pieceSizeY,
    size_t depth)
    : TextureExplosion(scene, tex, duration, velocityRect, acceleration, TextureExplosion::Size,
        tex->getSize().x/pieceSizeX, tex->getSize().y/pieceSizeY, depth) {}

void TextureExplosion::update(std::chrono::steady_clock::time_point curTime)
{
    if (lastTime == decltype(lastTime)()) lastTime = curTime;
    if (initialTime == decltype(initialTime)()) initialTime = curTime;
    if (curTime - initialTime > duration) remove();
    this->curTime = curTime;

    auto dt = toSeconds<float>(curTime - lastTime);

    for (auto& data : texturePieces)
    {
        data.position += data.velocity * dt;
        data.velocity += globalAcceleration * dt;
    }

    lastTime = curTime;
}

void TextureExplosion::render(Renderer& renderer)
{
    float opacity = 1.0f - toSeconds<float>(curTime - initialTime) / toSeconds<float>(duration);
    if (opacity < 0.0f) opacity = 0.0f;

    float pieceWidth = (float)texture->getSize().x/width;
    float pieceHeight = (float)texture->getSize().y/height;
    
    for (size_t i = 0; i < texturePieces.size(); i++)
    {
        for (size_t k = 0; k < 4; k++)
        {
            vertices[4*i+k].color.a = opacity * 255.0f;
            vertices[4*i+k].position = globalPosition + texturePieces[i].position;
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
