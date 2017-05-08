#include "PolyShape.h"
#include "Body.h"

namespace cp
{
    PolyShape::PolyShape(std::shared_ptr<Body> body, const std::vector<cpVect>& verts, cpFloat radius)
    : Shape(cpPolyShapeNewRaw(*body,
                              static_cast<int>(verts.size()),
                              const_cast<cpVect*>(reinterpret_cast<const cpVect*>(verts.data())),
                              radius),
            body)
    { }
}
