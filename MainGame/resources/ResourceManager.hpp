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

#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <thread>
#include <shared_mutex>
#include <condition_variable>
#include <generic_ptrs.hpp>
#include <non_copyable_movable.hpp>
#include "ResourceLocator.hpp"
#include "readerwriterqueue/readerwriterqueue.h"

class ResourceManager : util::non_copyable_movable
{
    moodycamel::BlockingReaderWriterQueue<std::string,32> loadCommandQueue;
    std::mutex cacheMutex;
    std::condition_variable newResourceLoaded;
    std::thread loadingThread;

    std::unordered_map<std::string,util::generic_shared_ptr> cache;
    std::unique_ptr<ResourceLocator> locator;

public:
    ResourceManager();
    ~ResourceManager();

    void loadLoop();
    void requestLoadAsync(std::string id);
    util::generic_shared_ptr load(std::string id);

    template <typename T>
    std::shared_ptr<T> load(std::string id) { return load(id).as<T>(); }

    void collectUnusedResources();

    ResourceLocator* getResourceLocator() { return locator.get(); }
    void setResourceLocator(ResourceLocator* loc) { locator.reset(loc); }
};
