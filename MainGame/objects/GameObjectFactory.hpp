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

#include "GameObject.hpp"

#include <type_traits>
#include <function_traits.hpp>
#include <generic_ptrs.hpp>
#include <streamReaders.hpp>
#include <string>

namespace
{
    template <typename Obj>
    class ConfigStructBase
    {
        static_assert(std::is_base_of<GameObject, Obj>::value, "You can only create a factory for a subclass of GameObject!");

        using ConfigureTraits = util::function_traits<decltype(&Obj::configure)>;
        static_assert(std::is_same<typename ConfigureTraits::return_type, bool>::value,
            "Incorrect return type of the configure method!");
        static_assert(ConfigureTraits::arity == 2, "Incorrect number of arguments of the configure method!");

        using ConfigArgument = typename ConfigureTraits::template argument<1>::type;
        static_assert(std::is_lvalue_reference<ConfigArgument>::value &&
            std::is_const<typename std::remove_reference_t<ConfigArgument>>::value &&
            !std::is_volatile<typename std::remove_reference_t<ConfigArgument>>::value,
            "Parameter of the configure method must be a const reference to a struct!");

    public:
        using value = std::remove_cv_t<std::remove_reference_t<ConfigArgument>>;
    };
}

template <typename Obj>
using ConfigStruct = typename ConfigStructBase<Obj>::value;

template <typename Obj>
util::generic_shared_ptr readerFor(sf::InputStream& stream)
{
	using namespace util;

    auto config = std::make_shared<ConfigStruct<Obj>>();
    if (!readFromStream(stream, *config)) return generic_shared_ptr{};
    return generic_shared_ptr{config};
}

class GameScene;

template <typename Obj>
std::unique_ptr<GameObject> factoryFor(GameScene& gameScene, std::string name, util::generic_shared_ptr parameters)
{
    auto params = parameters.try_convert<ConfigStruct<Obj>>();
    if (!params) return std::unique_ptr<GameObject>{};
    auto obj = std::make_unique<Obj>(gameScene);
    obj->setName(name);
    if (!obj->configure(*params)) return std::unique_ptr<GameObject>{};
    return std::unique_ptr<GameObject>(obj.release());
}

struct FactoryParams
{
    util::generic_shared_ptr (*reader)(sf::InputStream&);
    std::unique_ptr<GameObject> (*factory)(GameScene&, std::string, util::generic_shared_ptr);
};

struct BaseRegisterGameObject
{
    BaseRegisterGameObject(const std::string& name, FactoryParams factoryParams);
    ~BaseRegisterGameObject();
};

template <typename Obj>
struct RegisterGameObject : public BaseRegisterGameObject
{
    static_assert(std::is_base_of<GameObject, Obj>::value, "You can only register a subclass of GameObject!");
    RegisterGameObject(const std::string& name) : BaseRegisterGameObject(name, { readerFor<Obj>, factoryFor<Obj> }) {}
};

#define REG_BUILD_NAME2(c) __reg__##c
#define REG_BUILD_NAME(c) REG_BUILD_NAME2(c)
#define REGISTER_GAME_OBJECT(Obj) static RegisterGameObject<Obj> REG_BUILD_NAME(__COUNTER__)(#Obj)
