#ifndef CPPMUNK_SRC_CUSTOM_DELETER_H
#define CPPMUNK_SRC_CUSTOM_DELETER_H

#include <memory>

template <typename T, void (*f)(T*)>
struct CustomDeleter
{
    std::weak_ptr<T> weakAccess;
    void* userData;
    void operator(T* ptr)
    {
        f(ptr);
        weakAccess.reset();
    }
};

#endif
