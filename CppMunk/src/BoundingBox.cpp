#include "BoundingBox.h"

namespace Chipmunk
{
    BoundingBox::BoundingBox()
    {
        _boundingBox.l = 0.0f;
        _boundingBox.b = 0.0f;
        _boundingBox.r = 0.0f;
        _boundingBox.t = 0.0f;
    }
    
    BoundingBox::BoundingBox(cpBB box)
    {
        _boundingBox = box;
    }
    
    BoundingBox::BoundingBox(const cpFloat left,
                             const cpFloat bottom,
                             const cpFloat right,
                             const cpFloat top)
    {
        _boundingBox.l = left;
        _boundingBox.b = bottom;
        _boundingBox.r = right;
        _boundingBox.t = top;
    }
  
    BoundingBox::~BoundingBox()
    { }
    
    cpBool BoundingBox::intersects(const BoundingBox& box) const
    {
        return cpBBIntersects(_boundingBox, box.getBoundingBox());
    }
    
    cpBool BoundingBox::containsBoundingBox(const BoundingBox& box) const
    {
        return cpBBContainsBB(_boundingBox, box.getBoundingBox());
    }
    
    cpBool BoundingBox::containsVect(const cpVect& vector) const
    {
        return cpBBContainsVect(_boundingBox, vector);
    }
    
    cpVect BoundingBox::clampVect(const cpVect& vector) const
    {
        return cpBBClampVect(_boundingBox, vector);
    }
    
    cpVect BoundingBox::wrapVect(const cpVect& vector) const
    {
        return cpBBWrapVect(_boundingBox, vector);
    }
    
    cpBB BoundingBox::getBoundingBox() const
    {
        return _boundingBox;
    }
}