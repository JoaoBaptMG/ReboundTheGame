#pragma once

#include <iostream>
#include <memory>

class ResourceLocator
{
    ResourceLocator(const ResourceLocator&) = delete;
    void operator=(const ResourceLocator&) = delete;

public:
    ResourceLocator() {}
    virtual ~ResourceLocator() {}

    virtual std::unique_ptr<std::istream> getResource(std::string name) = 0;
};

