#ifndef CHIPMUNK_SHAPE_H
#define CHIPMUNK_SHAPE_H

#include <chipmunk/chipmunk.h>
#include <memory>

namespace cp
{
    class Body;
    class Space;
    class BoundingBox;
    
    class Shape
    {
    public:
        virtual ~Shape();
        operator cpShape*() const;
        
        /// Update, cache and return the bounding box of a shape based on the body it's attached to.
        BoundingBox cacheBoundingBox();
        /// Update, cache and return the bounding box of a shape with an explicit transformation.
        BoundingBox updateBoundingBox(cpTransform transform);
        
        /// Perform a nearest point query. It finds the closest point on the surface of shape to a specific point.
        /// The value returned is the distance between the points. A negative distance means the point is inside the shape.
        bool pointQuery(cpVect) const;

        /// Perform a segment query against a shape. @c info must be a pointer to a valid cpSegmentQueryInfo structure.
        bool segmentQuery(cpVect a, cpVect b, cpSegmentQueryInfo* = nullptr);

        /// Return contact information about two shapes.
        cpContactPointSet shapesCollide(std::shared_ptr<Shape> shapeA,
                                        std::shared_ptr<Shape> shapeB) { return cpShapesCollide(*shapeA, *shapeB); };
        
        /// The cpBody this shape is connected to.
        inline std::shared_ptr<Body> getBody() { return _body; };
        /// Set the cpBody this shape is connected to.
        /// Can only be used if the shape is not currently added to a space.
        void setBody(std::shared_ptr<Body> body);
        
        /// Get the mass of the shape if you are having Chipmunk calculate mass properties for you.
        inline cpFloat getMass() { return cpShapeGetMass(_shape); };
        /// Set the mass of this shape to have Chipmunk calculate mass properties for you.
        inline void setMass(cpFloat mass) { cpShapeSetMass(_shape, mass); };

        /// Get the density of the shape if you are having Chipmunk calculate mass properties for you.
        inline cpFloat getDensity() { return cpShapeGetDensity(_shape); };
        /// Set the density  of this shape to have Chipmunk calculate mass properties for you.
        inline void setDensity(cpFloat density) { cpShapeSetDensity(_shape, density); };

        /// Get the calculated moment of inertia for this shape.
        inline cpFloat getMoment() { return cpShapeGetMoment(_shape); };
        /// Get the calculated area of this shape.
        inline cpFloat getArea() { return cpShapeGetArea(_shape); };
        /// Get the centroid of this shape.
        inline cpVect getCenterOfGravity() { return cpShapeGetCenterOfGravity(_shape); };

        /// Get the bounding box that contains the shape given it's current position and angle.
        BoundingBox getBoundingBox();

        /// Get if the shape is set to be a sensor or not.
        inline cpBool isSensor() { return cpShapeGetSensor(_shape); };
        /// Set if the shape is a sensor or not.
        inline void setSensor(cpBool sensor) { cpShapeSetSensor(_shape, sensor); };

        /// Get the elasticity of this shape.
        inline cpFloat getElasticity() const { return cpShapeGetElasticity(_shape); };
        /// Set the elasticity of this shape.
        inline void setElasticity(cpFloat elasticity) { cpShapeSetElasticity(_shape, elasticity); };

        /// Get the friction of this shape.
        inline cpFloat getFriction() const { return cpShapeGetFriction(_shape); };
        /// Set the friction of this shape.
        inline void setFriction(cpFloat friction) { cpShapeSetFriction(_shape, friction); };

        /// Get the surface velocity of this shape.
        inline cpVect getSurfaceVelocity() const { return cpShapeGetSurfaceVelocity(_shape); };
        /// Set the surface velocity of this shape.
        inline void setSurfaceVelocity(cpVect surfaceVelocity) { cpShapeSetSurfaceVelocity(_shape, surfaceVelocity); }

        /// Get the user definable data pointer of this shape.
        inline cpDataPointer getUserData() { return cpShapeGetUserData(_shape); };
        /// Set the user definable data pointer of this shape.
        inline void setUserData(cpDataPointer data) { cpShapeSetUserData(_shape, data); };

        /// Set the collision type of this shape.
        inline cpCollisionType getCollisionType() const { return cpShapeGetCollisionType(_shape); };
        /// Get the collision type of this shape.
        inline void setCollisionType(cpCollisionType type) { cpShapeSetCollisionType(_shape, type); };

        /// Get the collision filtering parameters of this shape.
        inline cpShapeFilter getFilter() const { return cpShapeGetFilter(_shape); };
        /// Set the collision filtering parameters of this shape.
        inline void setFilter(cpShapeFilter filter) { cpShapeSetFilter(_shape, filter); };
        
    protected:
        Shape(cpShape*, std::shared_ptr<Body>);
        cpShape* _shape;
        
        std::shared_ptr<Body> _body;
    private:
        Shape(const Shape&);
        const Shape& operator=(const Shape&);
    };
}


#endif /* CHIPMUNK_SHAPE_H */
