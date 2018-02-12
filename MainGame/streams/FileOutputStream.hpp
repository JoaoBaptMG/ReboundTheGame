#pragma once

#include <cstdio>
#include <string>
#include <OutputStream.hpp>

class FileOutputStream final : public OutputStream
{
    std::FILE* file;

public:
    FileOutputStream() : file(nullptr) {}
    ~FileOutputStream();

    bool open(const std::string& filename);
    bool openForAppending(const std::string& filename);
    virtual bool write(const void* data, size_t size) override;
};
