#include "GUIMeter.hpp"

constexpr float MeterWidthNormal = 28;
constexpr float MeterBorderNormal = 4;

constexpr float MeterWidthSmall = 18;
constexpr float MeterBorderSmall = 2;

GUIMeter::GUIMeter(MeterSize size) : size(size)
{
    float width = size == MeterSize::Normal ? MeterWidthNormal : MeterWidthSmall;

    setQuadColor(0, sf::Color::Black);
    setQuadColor(4, sf::Color::White);

    for (size_t i = 0; i < 20; i++)
        vertices[i].texCoords = sf::Vector2f(0, 0);

    vertices[4].texCoords = sf::Vector2f(0, 0);
    vertices[5].texCoords = sf::Vector2f(width, 0);
    vertices[6].texCoords = sf::Vector2f(width, width);
    vertices[7].texCoords = sf::Vector2f(0, width);

    updateVertices();
}

void GUIMeter::setRect(size_t ind, sf::FloatRect rect)
{
    vertices[ind].position = sf::Vector2f(rect.left, rect.top);
    vertices[ind+1].position = sf::Vector2f(rect.left + rect.width, rect.top);
    vertices[ind+2].position = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
    vertices[ind+3].position = sf::Vector2f(rect.left, rect.top + rect.height);
}

void GUIMeter::setQuadColor(size_t ind, sf::Color color)
{
    vertices[ind].color = color;
    vertices[ind+1].color = color;
    vertices[ind+2].color = color;
    vertices[ind+3].color = color;
}

void GUIMeter::update(std::chrono::steady_clock::time_point)
{
    if (current != target)
    {
        if (current < target) current = target;
        else current--;

        updateVertices();
    }
}

void GUIMeter::updateVertices()
{
    float width = size == MeterSize::Normal ? MeterWidthNormal : MeterWidthSmall;
    float border = size == MeterSize::Normal ? MeterBorderNormal : MeterBorderSmall;

    float frameHeight = height + width + 3*border;
    float frameOffset = width + 2*border;
    setRect(0, sf::FloatRect(0, -frameHeight, width + 2*border, frameHeight));
    setRect(4, sf::FloatRect(border, -border-width, width, width));
    setRect(8, sf::FloatRect(border, -frameOffset-height, width, height));
    setRect(12, sf::FloatRect(border, -frameOffset-current, width, current));
    setRect(16, sf::FloatRect(border, -frameOffset-target, width, target));

    setQuadColor(8, backdropColor);
    setQuadColor(12, targetColor);
    setQuadColor(16, fillColor);
}

void GUIMeter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform.translate(position);
    states.texture = icon.get();
    target.draw(vertices, 20, sf::Quads, states);
}
