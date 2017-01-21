#include "FilesystemResourceLocator.hpp"
#include <cstdio>
#include <vector>

#if _WIN32
#elif __APPLE__
#elif __linux__
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

std::string getResourceDirectory()
{
    std::vector<char> buffer;

    ssize_t size = 8;
    do
    {
        size *= 2;
        buffer.resize(size, 0);
        size = readlink("/proc/self/exe", buffer.data(), buffer.size());
    } while (size == buffer.size());

    if (size < 0) return std::string{};

    for (; size > 1; size--)
        if (buffer[size] == '/') break;

    return std::string(buffer.data(), size) + "/Resources";
}
#endif

std::unique_ptr<sf::InputStream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<sf::FileInputStream> ptr{new sf::FileInputStream()};
    if (!ptr->open(fullname))
        throw FileNotFound(name);

    return std::unique_ptr<sf::InputStream>{ptr.release()};
}
