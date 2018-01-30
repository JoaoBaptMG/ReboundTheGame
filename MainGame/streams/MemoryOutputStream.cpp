#include "MemoryOutputStream.hpp"

#include <algorithm>

bool MemoryOutputStream::write(const void* data, size_t size)
{
    try
    {
        auto prevSize = contents.size();
        contents.resize(prevSize + size);
        std::memcpy(&contents[prevSize], data, size);
        return true;
    }
    catch (std::bad_alloc&)
    {
        return false;
    }
}

void MemoryOutputStream::alignTo(size_t align)
{
    // PKCS7 alignment
    auto newSize = ((contents.size() + align - 1) / align) * align;
    auto diff = newSize - contents.size();
    contents.resize(newSize, diff);
}
