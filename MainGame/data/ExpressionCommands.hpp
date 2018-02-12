#pragma once

#include <vector>
#include <SFML/System.hpp>

struct ExpressionCommands final
{
    std::vector<size_t> opcodes;
    
    #ifndef NDEBUG
    std::string prettyPrint();
    #endif
};

bool readFromStream(sf::InputStream &stream, ExpressionCommands& cmds);
intmax_t runExpression(const ExpressionCommands& cmds, size_t x);
