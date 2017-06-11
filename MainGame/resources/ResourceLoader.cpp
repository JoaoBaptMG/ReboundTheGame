#include "ResourceLoader.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

#include "data/RoomData.hpp"
#include "data/LevelData.hpp"
#include "data/TileSet.hpp"
#include "particles/ParticleEmitter.hpp"

using namespace util;
using namespace ResourceLoader;

using loadFunc = generic_shared_ptr (*)(std::unique_ptr<sf::InputStream>&);

template <typename T>
generic_shared_ptr loadGenericResource(std::unique_ptr<sf::InputStream>& stream)
{
    std::shared_ptr<T> content{new T()};
    if (content->loadFromStream(*stream))
        return generic_shared_ptr{content};

    return generic_shared_ptr{};
}

const std::unordered_map<std::string,loadFunc> loadFuncs =
{
    { "ts",  loadGenericResource<TileSet> },
    { "lvl", loadGenericResource<LevelData> },
    { "pe",  loadParticleEmitterList },
    { "map", loadGenericResource<RoomData> },
    { "png", loadGenericResource<sf::Texture> },
    { "ttf", loadGenericResource<sf::Font> }
};

generic_shared_ptr ResourceLoader::loadFromStream(std::unique_ptr<sf::InputStream> stream, std::string type)
{
    auto it = loadFuncs.find(type);
    if (it != loadFuncs.end())
        return it->second(stream);
    return generic_shared_ptr{};
}
