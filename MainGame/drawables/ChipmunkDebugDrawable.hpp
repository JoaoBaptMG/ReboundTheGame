#pragma once

#include <SFML/Graphics.hpp>
#include <cppmunk/Space.h>

class ChipmunkDebugDrawable final : public sf::Drawable
{
    const Chipmunk::Space& debugSpace;

public:
    ChipmunkDebugDrawable(const Chipmunk::Space& space) : debugSpace(space) {}

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
