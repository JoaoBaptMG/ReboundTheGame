#include "FilesystemResourceLocator.hpp"
#include <fstream>

#if __APPLE__
#else
inline static std::string getResourceDirectory()
{
    return getExecutableDirectory() + "/Resources";
}
#endif

FilesystemResourceLocator::FilesystemResourceLocator() : basename(getResourceDirectory())
{
}

std::unique_ptr<sf::InputStream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<sf::FileInputStream> ptr{new sf::FileInputStream()};
    if (!ptr->open(fullname))
        throw FileNotFound(name);

    return std::unique_ptr<sf::InputStream>{ptr.release()};
}
