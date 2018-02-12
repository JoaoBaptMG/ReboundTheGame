#include "FontHandler.hpp"

using namespace util;

FontHandler::FontHandler(std::unique_ptr<const char[]> &&data, size_t size) : fontData(std::move(data)),
    harfBuzzWrapper(fontData.get(), size)
{
    if (!font.loadFromMemory(fontData.get(), size))
        throw -1;
}

util::generic_shared_ptr loadFontHandler(std::unique_ptr<sf::InputStream>& stream)
{
    try
    {
        auto size = stream->getSize();
        if (size == -1) return generic_shared_ptr{};
        if (stream->seek(0) != 0) return generic_shared_ptr{};
        
        std::unique_ptr<char[]> memory(new char[size]);
        if (stream->read(memory.get(), size) != size)
            return generic_shared_ptr{};
            
        return generic_shared_ptr{std::make_shared<FontHandler>(std::move(memory), size)};
    }
    catch (...)
    {
        return generic_shared_ptr{};
    }
}
