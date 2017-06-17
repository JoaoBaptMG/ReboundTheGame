#pragma once

#include <SFML/System.hpp>
#include <iostream>

class STLStream final : public sf::InputStream
{
    std::istream& wrapped;

public:
    STLStream(std::istream& wrapped) : wrapped(wrapped) {}

    virtual sf::Int64 read(void *data, sf::Int64 size) override;
    virtual sf::Int64 seek(sf::Int64 position) override;
    virtual sf::Int64 tell() override;
    virtual sf::Int64 getSize() override;

    virtual ~STLStream() {}
};
