#pragma once

struct VarLength
{
    size_t& target;
    
private:
    VarLength(size_t& t) : target(t) {}
    
public:
    friend VarLength varLength(size_t& t)
    { 
        return VarLength(t);
    }
    
    friend const VarLength varLength(const size_t& t)
    {
        return VarLength(const_cast<size_t&>(t));
    }
};

VarLength varLength(size_t& t);
const VarLength varLength(const size_t& t);

template <typename T>
struct is_optimization_viable : public std::is_trivial<T> {};
