#pragma once

#include "ResourceLocator.hpp"
#include <stdexcept>

std::string getResourceDirectory();

class FilesystemResourceLocator : public ResourceLocator
{
    std::string basename;

public:
    FilesystemResourceLocator() : basename(getResourceDirectory()) {}
    virtual ~FilesystemResourceLocator() {}

    virtual std::unique_ptr<sf::InputStream> getResource(std::string name) override;
};

class FileNotFound : std::runtime_error
{
public:
    inline FileNotFound(std::string name)
    : std::runtime_error("File " + name + " not found!") {}
};

