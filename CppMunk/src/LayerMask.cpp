#include "LayerMask.h"

#include <cassert>

namespace Chipmunk
{
    LayerMask::LayerMask() :
    _bitMask(0)
    { }
    
    LayerMask::LayerMask(unsigned int layer) :
    _bitMask(1 << layer)
    {
        assert(layer <= 31);
    }
    
    LayerMask::operator cpBitmask() const
    {
        return _bitMask;
    }
    
    LayerMask LayerMask::operator~() const
    {
        LayerMask tmp;
        tmp._bitMask = ~_bitMask;
        return tmp;
    }
    
    LayerMask LayerMask::operator|(LayerMask rhs) const
    {
        LayerMask tmp;
        tmp._bitMask = _bitMask | rhs._bitMask;
        return tmp;
    }
}
