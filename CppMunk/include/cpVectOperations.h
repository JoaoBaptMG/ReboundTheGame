#ifndef CHIPMUNK_CPVECT_OPERATIONS_H
#define CHIPMUNK_CPVECT_OPERATIONS_H

#include <chipmunk/chipmunk.h>

inline static cpVect operator*(const cpFloat s, const cpVect v) { return v*s; }

inline static cpVect& operator+=(cpVect& v1, cpVect v2) { return v1 = v1 + v2; }
inline static cpVect& operator-=(cpVect& v1, cpVect v2) { return v1 = v1 - v2; }
inline static cpVect& operator*=(cpVect& v, cpFloat s) { return v = v * s; }

inline static bool operator!=(cpVect v1, cpVect v2) { return !(v1 == v2); }

#endif /* CHIPMUNK_CPVECT_OPERATIONS_H */
