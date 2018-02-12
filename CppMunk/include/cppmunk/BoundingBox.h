#ifndef CHIPMUNK_BOUNDINGBOX_H
#define CHIPMUNK_BOUNDINGBOX_H

#include <chipmunk/chipmunk.h>

namespace cp
{
    class BoundingBox
    {
    public:
        BoundingBox();
        BoundingBox(cpBB box);
        BoundingBox(const cpFloat left, const cpFloat bottom, const cpFloat right, const cpFloat top);
        ~BoundingBox();
        
        cpBool intersects(const BoundingBox& box) const;
        cpBool containsBoundingBox(const BoundingBox& box) const;
        cpBool containsVect(const cpVect& vector) const;
        cpVect clampVect(const cpVect& vector) const;
        cpVect wrapVect(const cpVect& vector) const;
        cpBB getBoundingBox() const;
        
    private:
        cpBB _boundingBox;
    };
}



#endif /* CHIPMUNK_BOUNDINGBOX_H */
