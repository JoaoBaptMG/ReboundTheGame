#include "GUIMeter.hpp"

constexpr float MeterWidthNormal = 32;
constexpr float MeterBorderNormal = 4;

constexpr float MeterWidthSmall = 14;
constexpr float MeterBorderSmall = 2;

GUIMeter::GUIMeter(MeterSize size) : size(size)
{
    float width = size == MeterSize::Normal ? MeterWidthNormal : MeterWidthSmall;

    setQuadColor(0, sf::Color::Black);
    setQuadColor(4, sf::Color(200, 200, 200));

    for (size_t i = 0; i < 16; i++)
        vertices[i].texCoords = sf::Vector2f(0, 0);

    vertices[16].texCoords = sf::Vector2f(0, 0);
    vertices[17].texCoords = sf::Vector2f(width, 0);
    vertices[18].texCoords = sf::Vector2f(width, width);
    vertices[19].texCoords = sf::Vector2f(0, width);

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
    setRect(4, sf::FloatRect(border, -frameOffset-(float)height, width, (float)height));
    setRect(8, sf::FloatRect(border, -frameOffset-(float)current, width, (float)current));
    setRect(12, sf::FloatRect(border, -frameOffset-(float)target, width, (float)target));
    setRect(16, sf::FloatRect(border, -border-width, width, width));

    setQuadColor(4, backdropColor);
    setQuadColor(8, targetColor);
    setQuadColor(12, fillColor);
}

void GUIMeter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform.translate(position);
    target.draw(vertices, 20, sf::Quads, states);
    
    if (icon)
    {
        bool wasSmooth = icon->isSmooth();
        if (!wasSmooth) icon->setSmooth(true);
        states.texture = icon.get();
        target.draw(vertices+16, 4, sf::Quads, states);
        if (wasSmooth) icon->setSmooth(false);
    }
}
