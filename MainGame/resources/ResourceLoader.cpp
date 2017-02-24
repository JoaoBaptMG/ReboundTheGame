#include "ResourceLoader.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include "data/RoomData.hpp"
#include "data/LevelData.hpp"

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

generic_shared_ptr loadStringMap(std::unique_ptr<sf::InputStream>& stream)
{
    auto size = stream->getSize();
    if (size == -1) return generic_shared_ptr{};

    std::vector<char> buffer(size, (char)0);
    auto err = stream->read(buffer.data(), buffer.size());

    if (err == -1) return generic_shared_ptr{};

    std::shared_ptr<std::string> str{new std::string{buffer.begin(), buffer.end()}};
    return generic_shared_ptr{str};
}

const loadFunc loadFunctions[] =
{
    loadGenericResource<LevelData>,
    loadGenericResource<RoomData>,
    loadGenericResource<sf::Texture>,
    loadGenericResource<sf::Font>,
    loadStringMap
};

generic_shared_ptr ResourceLoader::loadFromStream(std::unique_ptr<sf::InputStream> stream)
{
    for (auto func : loadFunctions)
    {
        auto ptr = func(stream);
        if (ptr) return ptr;
        stream->seek(0);
    }

    return generic_shared_ptr{};
}
