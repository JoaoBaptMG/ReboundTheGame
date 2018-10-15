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


#include "LevelPersistentData.hpp"

LevelPersistentData::LevelPersistentData()
{
}

void LevelPersistentData::setDataImpl(std::string key, void* ptr, void(*deleter)(void*), util::type_id_t type)
{
    auto it = data.find(key);
    
    if (it == data.end())
        data.emplace(key, DataDetail{ DataDetail::Ptr{ptr, deleter}, type });
    else if (it->second.type == type)
        it->second.ptr.reset(ptr);
    else it->second.ptr = DataDetail::Ptr{ptr, deleter};
}

void* LevelPersistentData::getDataImpl(std::string key, util::type_id_t type)
{
    auto it = data.find(key);
    
    if (it == data.end())
        throw InvalidLevelPersistentDataAccessError(key, "data does not exist!");
    else if (it->second.type != type)
        throw InvalidLevelPersistentDataAccessError(key, "data has invalid type!");
    else return it->second.ptr.get();
}

bool LevelPersistentData::existsDataOfTypeImpl(std::string key, util::type_id_t type)
{
    auto it = data.find(key);
    return it != data.end() && it->second.type == type;
}

void LevelPersistentData::clear()
{
    data.clear();
}
    
