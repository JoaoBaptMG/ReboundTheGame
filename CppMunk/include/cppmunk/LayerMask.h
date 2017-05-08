#ifndef CHIPMUNK_LAYERMASK_H
#define CHIPMUNK_LAYERMASK_H

#include <chipmunk/chipmunk.h>

namespace cp
{
    class LayerMask
    {
    public:
        LayerMask();
        LayerMask(unsigned int layer);
        LayerMask operator~() const;
        LayerMask operator|(LayerMask rhs) const;
        explicit operator cpBitmask() const;
    private:
        cpBitmask _bitMask;
    };
    
    const static LayerMask ALL_LAYERS(~(LayerMask()));
}


#endif /* CHIPMUNK_LAYERMASK_H */
