#pragma once

#include <string>
#include <SFML/System.hpp>

#include "LocalizationManager.hpp"

std::string scanCodeToKeyName(sf::Uint32 code, LocalizationManager& lm);
