#pragma once

#include "ResourceLocator.hpp"
#include "execDir.hpp"
#include <stdexcept>

class FilesystemResourceLocator : public ResourceLocator
{
    std::string basename;

public:
    FilesystemResourceLocator();
    virtual ~FilesystemResourceLocator() {}

    virtual std::unique_ptr<sf::InputStream> getResource(std::string name) override;
};

class FileNotFound : public std::runtime_error
{
public:
    inline FileNotFound(std::string name)
    : std::runtime_error("File " + name + " not found!") {}
};

