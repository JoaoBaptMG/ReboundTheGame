//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "ResourceLoader.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

#include "data/RoomData.hpp"
#include "data/LevelData.hpp"
#include "data/TileSet.hpp"
#include "particles/ParticleEmitter.hpp"
#include "audio/readWav.hpp"

#include "FontHandler.hpp"

using namespace util;
using namespace ResourceLoader;

using loadFunc = generic_shared_ptr (*)(std::unique_ptr<sf::InputStream>&);

template <typename T>
generic_shared_ptr loadGenericResource(std::unique_ptr<sf::InputStream>& stream)
{
    std::shared_ptr<T> content{new T()};
	if (checkMagic(*stream, T::ReadMagic) && readFromStream(*stream, *content))
		return generic_shared_ptr{content};

    return generic_shared_ptr{};
}

template <typename T>
generic_shared_ptr loadSFMLResource(std::unique_ptr<sf::InputStream>& stream)
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
    { "png", loadSFMLResource<sf::Texture> },
    { "ttf", loadFontHandler },
    { "wav", loadWaveFile },
};

generic_shared_ptr ResourceLoader::loadFromStream(std::unique_ptr<sf::InputStream> stream, std::string type)
{
    auto it = loadFuncs.find(type);
	if (it != loadFuncs.end())
		return it->second(stream);
    return generic_shared_ptr{};
}
