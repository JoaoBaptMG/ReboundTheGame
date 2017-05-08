#pragma once

#include "utility/non_copyable_movable.hpp"
#include "utility/generic_ptrs.hpp"
#include <memory>
#include <chrono>
#include <SFML/System.hpp>

class GameScene;
class Renderer;

class GameObject : util::non_copyable
{
protected:
    GameScene& gameScene;
    bool shouldRemove;
    bool isPersistent;

    std::string name;

public:
    GameObject(GameScene& scene) : gameScene(scene), shouldRemove(false), isPersistent(false), name() {}
    inline void remove() { shouldRemove = true; }

    virtual void update(std::chrono::steady_clock::time_point curTime) = 0;
    virtual void render(Renderer& renderer) = 0;

    std::string getName() const { return name; }
    void setName(std::string name) { this->name = name; }

    virtual ~GameObject() {}

    friend class GameScene;
};

struct GameObjectDescriptor;

util::generic_shared_ptr readParametersFromStream(sf::InputStream& stream, std::string klass);
std::unique_ptr<GameObject> createObjectFromDescriptor(GameScene& gameScene, const GameObjectDescriptor& descriptor);
