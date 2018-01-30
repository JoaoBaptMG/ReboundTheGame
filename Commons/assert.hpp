#pragma once

#include <string>
#include <stdexcept>

class assert_exception : public std::exception
{
    std::string msg;

public:
    assert_exception(const std::string &expr) : msg("Assertion failed: " + expr) {}
    virtual ~assert_exception() {}

    virtual const char* what() const noexcept override
    {
        return msg.c_str();
    }
};

static inline void _detail_builtin_assert(bool value, std::string expression)
{
    if (!value) throw assert_exception(expression);
}

#define ASSERT(val) _detail_builtin_assert(val, #val)
