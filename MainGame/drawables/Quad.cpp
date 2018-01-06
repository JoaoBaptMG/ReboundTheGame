#include "Quad.hpp"

#include <assert.hpp>

Quad::Quad(sf::FloatRect rect, sf::Color color) : rect(rect), color(color), vertices(sf::PrimitiveType::TriangleFan)
{
    setupVertices();
}

void Quad::setupVertices()
{
    vertices.resize(4);
    
    vertices[0] = sf::Vertex(sf::Vector2f(rect.left, rect.top), color);
    vertices[1] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top), color);
    vertices[2] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top + rect.height), color);
    vertices[3] = sf::Vertex(sf::Vector2f(rect.left, rect.top + rect.height), color);
}

void Quad::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(vertices, states);
}
