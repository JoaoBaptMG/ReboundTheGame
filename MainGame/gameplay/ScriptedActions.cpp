#include "ScriptedActions.hpp"

bool ScriptedButtonAction::isTriggered() const { return false; }
bool ScriptedButtonAction::isPressed() const { return false; }
bool ScriptedButtonAction::isReleased() const { return false; }

sf::Vector2f ScriptedDualAxisAction::getValue() const { return sf::Vector2f(0, 0); }
