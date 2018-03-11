#pragma once

#include <SFML/System.hpp>
#include <string>

#include "objects/InteractableObject.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/SegmentedSprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "gameplay/Script.hpp"

class GameScene;
class Renderer;

namespace props
{
    class MessageSign : public ::InteractableObject
    {
        Sprite signPole;
        SegmentedSprite signBox;
        TextDrawable signLabel;
        std::string messageString;

    public:
        MessageSign(GameScene& scene);
        virtual ~MessageSign() = default;

        virtual void interact() override;
        virtual void render(Renderer& renderer) override;

        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            std::string messageString;
        };

        bool configure(const ConfigStruct& config);

        auto getDisplayPosition()
        {
            auto pos = interactionCenter;
            return sf::Vector2f(roundf(pos.x), roundf(pos.y+40));
        }
    };

    bool readFromStream(sf::InputStream& stream, MessageSign::ConfigStruct& config);
}