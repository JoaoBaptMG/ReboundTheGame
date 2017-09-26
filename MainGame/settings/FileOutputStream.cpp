#include "FileOutputStream.hpp"

FileOutputStream::~FileOutputStream()
{
    if (file) std::fclose(file);
}

bool FileOutputStream::open(const std::string& filename)
{
    if (file) std::fclose(file);
    return (file = std::fopen(filename.c_str(), "wb")) != nullptr;
}

bool FileOutputStream::openForAppending(const std::string& filename)
{
    if (file) std::fclose(file);
    return (file = std::fopen(filename.c_str(), "ab")) != nullptr;
}

bool FileOutputStream::write(const void* data, size_t size)
{
    if (!file) return false;
    return std::fwrite(data, size, 1, file) > 0;
}
