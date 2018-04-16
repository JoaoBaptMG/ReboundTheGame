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


#include "ResourceManager.hpp"
#include "ResourceLoader.hpp"

using namespace util;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

generic_shared_ptr ResourceManager::load(std::string id)
{
    auto it = cache.find(id);

    if (it == cache.end())
    {
        auto type = id.substr(id.find_last_of('.') + 1);
        auto ptr = ResourceLoader::loadFromStream(locator->getResource(id), type);
        if (!ptr) throw ResourceLoadingError(id);
        std::tie(it, std::ignore) = cache.emplace(id, ptr);
    }

    return it->second;
}

void ResourceManager::collectUnusedResources()
{
    for (auto it = cache.begin(); it != cache.end();)
    {
        if (it->second.use_count() == 1)
            it = cache.erase(it);
        else ++it;
    }
}
