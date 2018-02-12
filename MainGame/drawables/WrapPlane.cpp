#include "WrapPlane.hpp"

void WrapPlane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Vertex quad[4];

    quad[0].position = sf::Vector2f(drawArea.left, drawArea.top);
    quad[1].position = sf::Vector2f(drawArea.left, drawArea.top + drawArea.height);
    quad[2].position = sf::Vector2f(drawArea.left + drawArea.width, drawArea.top + drawArea.height);
    quad[3].position = sf::Vector2f(drawArea.left + drawArea.width, drawArea.top);

    auto invTransform = states.transform.getInverse();

    for (auto& vtx : quad)
    {
        vtx.color = sf::Color::White;
        vtx.texCoords = invTransform.transformPoint(vtx.position);
    }

    states.transform = sf::Transform::Identity;
    states.texture = texture.get();

    target.draw(quad, 4, sf::TriangleFan, states);
}
