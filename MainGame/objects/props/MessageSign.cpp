#include "MessageSign.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "gameplay/ScriptedPlayerController.hpp"
#include <streamReaders.hpp>

using namespace props;

MessageSign::MessageSign(GameScene& scene)
    : InteractableObject(scene),
      signPole(scene.getResourceManager().load<sf::Texture>("sign-pole.png")),
      signBox(scene.getResourceManager().load<sf::Texture>("sign-background.png")),
      signLabel(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName()))
{
    auto& lm = scene.getLocalizationManager();

    signLabel.setString(lm.getString("message-sign-display"));
    signLabel.setFontSize(48);
    signLabel.setDefaultColor(sf::Color::Black);
    signLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    signLabel.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(signLabel, lm);
    signLabel.buildGeometry();

    auto bounds = signLabel.getLocalBounds();
    if (bounds.width < 48) bounds.width = 48;
    if (bounds.height < 48) bounds.height = 48;

    signBox.setCenterRect(sf::FloatRect(4, 4, 4, 4));
    signBox.setDestinationRect(sf::FloatRect(0, 0, bounds.width + 16, bounds.height + 16));
    signBox.setAnchorPoint(sf::Vector2f(bounds.width/2 + 8, bounds.height/2 + 8));

    auto size = signPole.getTextureSize();
    signPole.setAnchorPoint(sf::Vector2f(size.x/2, size.y));

    interactionRadius = 40;

    callbackEntry = lm.registerLanguageChangeCallback([=,&lm]
    {
        signLabel.setString(lm.getString("message-sign-display"));
        configTextDrawable(signLabel, lm);
        signLabel.buildGeometry();

        auto bounds = signLabel.getLocalBounds();
        if (bounds.width < 48) bounds.width = 48;
        if (bounds.height < 48) bounds.height = 48;

        signBox.setDestinationRect(sf::FloatRect(0, 0, bounds.width + 16, bounds.height + 16));
        signBox.setAnchorPoint(sf::Vector2f(bounds.width/2 + 8, bounds.height/2 + 8));
    });
}

bool props::readFromStream(sf::InputStream& stream, MessageSign::ConfigStruct& config)
{
    return ::readFromStream(stream, config.position, config.messageString);
}

bool MessageSign::configure(const MessageSign::ConfigStruct& config)
{
    interactionCenter = cpv(config.position.x, config.position.y - 40);
    float displacement = signPole.getTextureSize().y + 2*signBox.getAnchorPoint().y + 8;
    popupPosition = interactionCenter - cpv(0, displacement-40);

    messageString = config.messageString;

    return true;
}

void MessageSign::interact()
{
    gameScene.runCutsceneScript([=] (Script& script)
    {
        if (messageString[0] == 2)
        {
            const auto& keyMap = gameScene.getKeySpecifierMap();
            gameScene.getMessageBox().displayFormattedString(script, messageString.substr(1), {}, {}, keyMap);
        }
        else gameScene.getMessageBox().displayString(script, messageString);
    });
}

void MessageSign::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(signPole, {}, 13);
    renderer.currentTransform.translate(0, -(float)signPole.getTextureSize().y - signBox.getAnchorPoint().y);
    renderer.pushDrawable(signBox, {}, 13);
    renderer.pushDrawable(signLabel, {}, 14);
    renderer.popTransform();
}

