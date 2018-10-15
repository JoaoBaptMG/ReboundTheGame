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

#include <type_id.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <exception>

class LevelPersistentData final
{
    struct DataDetail
    {
        using Ptr = std::unique_ptr<void, void(*)(void*)>;
        Ptr ptr;
        util::type_id_t type;
    };
    
    std::unordered_map<std::string,DataDetail> data;
    
    void setDataImpl(std::string key, void* ptr, void(*deleter)(void*), util::type_id_t type);
    void* getDataImpl(std::string key, util::type_id_t type);
    bool existsDataOfTypeImpl(std::string key, util::type_id_t type);
    
public:
    LevelPersistentData();
    ~LevelPersistentData() {}
    
    template <typename T>
    void setData(std::string key, const T& data)
    {
        setDataImpl(key, new T(data), [](void* p) { delete static_cast<T*>(p); }, util::type_id<T>());
    }
    
    template <typename T>
    T getData(std::string key)
    {
        return *static_cast<T*>(getDataImpl(key, util::type_id<T>()));
    }
    
    template <typename T>
    bool existsDataOfType(std::string key)
    {
        return existsDataOfTypeImpl(key, util::type_id<T>());
    }
    
    void clear();
};

struct InvalidLevelPersistentDataAccessError final : public std::runtime_error
{
    InvalidLevelPersistentDataAccessError(std::string key, std::string msg) :
        std::runtime_error("Exception while accessing level persistent data with key " + key + ": " + msg) {}
};
