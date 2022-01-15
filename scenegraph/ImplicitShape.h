#ifndef IMPLICITSHAPE_H
#define IMPLICITSHAPE_H

#include "RayGeometry.h"


class ImplicitShape
{
public:
    ImplicitShape();
    Intersection nearestIntersection(std::vector<Intersection> intersections);
    IntersectionWithPrimitive nearestIntersectionWithPrimitive(std::vector<IntersectionWithPrimitive> intersections);
    float nonNegativeMin(std::vector<float> values);
    std::vector<float> solveQuadratic(float a, float b, float c);
    // Helper for cone and cylinder
    bool isWithinHorizontalCircle(glm::vec3 point);
};

#endif // IMPLICITSHAPE_H
