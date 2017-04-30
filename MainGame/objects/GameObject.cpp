#include "GameObject.hpp"

#include <type_traits>
#include <unordered_map>

#include "data/RoomData.hpp"
#include "utility/function_traits.hpp"
#include "utility/generic_ptrs.hpp"
#include "utility/streamCommons.hpp"

// Objects
#include "objects/Player.hpp"
#include "objects/enemies/Floater.hpp"
#include "objects/enemies/Rotator.hpp"
#include "objects/collectibles/HealthPickup.hpp"
#include "objects/collectibles/Powerup.hpp"
#include "objects/props/PushableCrate.hpp"
#include "objects/props/BombCrate.hpp"

namespace detail
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
            "Parameter of the configure method must be a const reference to a standard-layout struct!");

    public:
        using value = std::remove_cv_t<std::remove_reference_t<ConfigArgument>>;
    };
}

template <typename Obj>
using ConfigStruct = typename detail::ConfigStructBase<Obj>::value;

template <typename Obj>
auto readerFor(sf::InputStream& stream)
{
    using namespace util;
    
    auto config = std::make_shared<ConfigStruct<Obj>>();
    if (!readFromStream(stream, *config)) return util::generic_shared_ptr{};
    return util::generic_shared_ptr{config};
}

template <typename Obj>
auto factoryFor(GameScene& gameScene, util::generic_shared_ptr parameters)
{
    auto params = parameters.try_convert<ConfigStruct<Obj>>();
    if (!params) return std::unique_ptr<GameObject>{};
    auto obj = std::make_unique<Obj>(gameScene);
    if (!obj->configure(*params)) return std::unique_ptr<GameObject>{};
    return std::unique_ptr<GameObject>(obj.release());
}

struct FactoryParams
{
    util::generic_shared_ptr (*reader)(sf::InputStream&);
    std::unique_ptr<GameObject> (*factory)(GameScene&, util::generic_shared_ptr);
};

#define DEFINE_FACTORY(cls) { #cls, { readerFor<cls>, factoryFor<cls> } }

const std::unordered_map<std::string, FactoryParams> factoryParams =
{
    // the player
    DEFINE_FACTORY(Player),

    // enemies
    DEFINE_FACTORY(enemies::Floater),
    DEFINE_FACTORY(enemies::Rotator),

    // collectibles
    DEFINE_FACTORY(collectibles::HealthPickup),
    DEFINE_FACTORY(collectibles::Powerup),

    // props
    DEFINE_FACTORY(props::PushableCrate),
    DEFINE_FACTORY(props::BombCrate),
};

#undef DEFINE_FACTORY

util::generic_shared_ptr readParametersFromStream(sf::InputStream& stream, std::string klass)
{
    auto it = factoryParams.find(klass);

    if (it == factoryParams.end())
        return util::generic_shared_ptr{};

    return it->second.reader(stream);
}

std::unique_ptr<GameObject> createObjectFromDescriptor(GameScene& gameScene, const GameObjectDescriptor& descriptor)
{
    auto it = factoryParams.find(descriptor.klass);

    if (it == factoryParams.end())
        return std::unique_ptr<GameObject>{};

    return it->second.factory(gameScene, descriptor.parameters);
}
