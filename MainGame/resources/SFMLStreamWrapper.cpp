#include "SFMLStreamWrapper.hpp"

sf::Int64 STLStream::read(void *data, sf::Int64 size)
{
    wrapped.read((char*)data, size);
    return wrapped.gcount();
}

sf::Int64 STLStream::seek(sf::Int64 position)
{
    wrapped.seekg(std::streampos(position));
    return tell();
}

sf::Int64 STLStream::tell()
{
    return wrapped.tellg();
}

sf::Int64 STLStream::getSize()
{
    auto lastTell = wrapped.tellg();
    sf::Int64 result = 0;
    wrapped.seekg(std::streamoff(0), std::ios::end);
    result = tell();
    wrapped.seekg(lastTell);
    return result;
}
