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


#include "GameObjectFactory.hpp"

#include <new>
#include <unordered_map>
#include "data/RoomData.hpp"

#include <iostream>

using FactoryParamMap = std::unordered_map<std::string, FactoryParams>;
static size_t SchwartzCounter = 0;
static typename std::aligned_storage_t<sizeof(FactoryParamMap), alignof(FactoryParamMap)> factoryParamsBuf;
FactoryParamMap& factoryParams = reinterpret_cast<FactoryParamMap&>(factoryParamsBuf);

BaseRegisterGameObject::BaseRegisterGameObject(const std::string& name, FactoryParams params)
{
    if (SchwartzCounter++ == 0) new (&factoryParams) FactoryParamMap();
    factoryParams.emplace(name, params);

    std::cout << "Registering " << name << " into factory..." << std::endl;
}

BaseRegisterGameObject::~BaseRegisterGameObject()
{
    if (--SchwartzCounter == 0) (&factoryParams)->~FactoryParamMap();
}

util::generic_shared_ptr readParametersFromStream(sf::InputStream& stream, std::string klass)
{
    if (SchwartzCounter == 0) return util::generic_shared_ptr{};

    auto it = factoryParams.find(klass);

    if (it == factoryParams.end())
        return util::generic_shared_ptr{};

    return it->second.reader(stream);
}

std::unique_ptr<GameObject> createObjectFromDescriptor(GameScene& gameScene, const GameObjectDescriptor& descriptor)
{
    if (SchwartzCounter == 0) return std::unique_ptr<GameObject>{};

    auto it = factoryParams.find(descriptor.klass);

    if (it == factoryParams.end())
        return std::unique_ptr<GameObject>{};

    auto obj = it->second.factory(gameScene, descriptor.name, descriptor.parameters);
    return obj;
}
