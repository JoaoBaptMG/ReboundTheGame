#pragma once

#include <vector>
#include <string>
#include <OutputStream.hpp>

class MemoryOutputStream final : public OutputStream
{
    std::vector<uint8_t> contents;
    
public:
    MemoryOutputStream() {}
    ~MemoryOutputStream() {}
    
    virtual bool write(const void* data, size_t size) override;
    void alignTo(size_t align);
    auto& getContents() { contents.shrink_to_fit(); return contents; }
    const auto& getContents() const { return contents; }
};
