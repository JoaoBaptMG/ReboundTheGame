#include "GameObject.hpp"

#include <type_traits>
#include <unordered_map>

#include "data/RoomData.hpp"
#include "utility/function_traits.hpp"

// Objects
#include "objects/Player.hpp"
#include "objects/enemies/Floater.hpp"

template <typename Deduced> struct Print;

template <typename Obj>
auto factoryFor(GameScene& gameScene, const Memory& parameters)
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
        "Parameter of the configure method must be a const reference to a standard-layout struct!");

    using ConfigStruct = std::remove_cv_t<std::remove_reference_t<ConfigArgument>>;
    static_assert(std::is_standard_layout<ConfigStruct>::value,
        "Parameter of the configure method must be a const reference to a standard-layout struct!");

    if (sizeof(ConfigStruct) != parameters.get_size())
        return std::unique_ptr<GameObject>{};

    auto obj = std::make_unique<Obj>(gameScene);
    auto parameterPtr = reinterpret_cast<const ConfigStruct*>(parameters.get_ptr());

    if (!obj->configure(*parameterPtr)) return std::unique_ptr<GameObject>{};
    return std::unique_ptr<GameObject>{obj.release()};
}

using FactoryFunction = std::unique_ptr<GameObject>(*)(GameScene&, const Memory&);

#define DEFINE_FACTORY(cls) { #cls, factoryFor<cls> }

const std::unordered_map<std::string, FactoryFunction> factoryFunctions =
{
    // the player
    DEFINE_FACTORY(Player),

    // enemies
    DEFINE_FACTORY(enemies::Floater),
    
};

#undef DEFINE_FACTORY

std::unique_ptr<GameObject> createObjectFromDescriptor(GameScene& gameScene, const GameObjectDescriptor& descriptor)
{
    auto it = factoryFunctions.find(descriptor.klass);

    if (it == factoryFunctions.end())
        return std::unique_ptr<GameObject>{};

    return it->second(gameScene, descriptor.parameters);
}
