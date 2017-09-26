#pragma once

#include <cstdint>

class OutputStream
{
public:
    virtual bool write(const void* data, size_t size) = 0;
};
