#include "PolyShape.h"
#include "Body.h"

namespace Chipmunk
{
    PolyShape::PolyShape(std::shared_ptr<Body> body, const std::vector<cpVect>& verts)
    : Shape(cpPolyShapeNewRaw(*body,
                              static_cast<int>(verts.size()),
                              const_cast<cpVect*>(reinterpret_cast<const cpVect*>(&verts[0])),
                              0),
            body)
    { }
}
