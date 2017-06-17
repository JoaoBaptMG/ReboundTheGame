#include "FilesystemResourceLocator.hpp"
#include <fstream>

std::unique_ptr<std::istream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<std::istream> ptr{new std::ifstream(fullname, std::ios::binary)};
    if (!ptr->good())
        throw FileNotFound(name);

    return std::move(ptr);
}
