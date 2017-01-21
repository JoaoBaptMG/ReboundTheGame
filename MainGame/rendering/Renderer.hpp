#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <stack>
#include "../utility/non_copyable_movable.hpp"

using RenderData = std::pair<std::reference_wrapper<const sf::Drawable>,sf::RenderStates>;

class Renderer final : util::non_copyable_movable
{
    std::multimap<long,RenderData> drawableList;
    sf::RenderTarget& target;

    std::stack<sf::Transform> transformStack;

public:
    explicit Renderer(sf::RenderTarget& target) : target(target), currentTransform() {}

    void pushDrawable(const sf::Drawable &drawable, sf::RenderStates states, long depth = 0);

    void pushTransform();
    void popTransform();

    void render();
    void clearState();

    sf::Transform currentTransform;
};

