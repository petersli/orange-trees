#include "ImplicitShape.h"
#include "RayGeometry.h"

ImplicitShape::ImplicitShape()
{
}

/**
 *  Returns the intersection that has the smallest non-negative t-value.
 *  If all intersections have negative t-values, returns an intersection with t = -1
 */
Intersection ImplicitShape::nearestIntersection(std::vector<Intersection> intersections) {
    float minTValue = INFINITY;
    float minIndex = 0;
    for (int i = 0; i < intersections.size(); i++) {
        float t = intersections[i].t;
        if (t >= EPSILON && t < minTValue) {
            minTValue = t;
            minIndex = i;
        }
    }
    if (minTValue == INFINITY) {
        // Return null intersection (t = -1)
        return Intersection();
    } else {
        return intersections[minIndex];
    }
}


/**
 *  Returns the intersection that has the smallest non-negative t-value.
 *  If all intersections have negative t-values, returns an intersection with t = -1
 *  Note: same as nearestIntersection, just handles IntersectionWithPrimitive type
 */
IntersectionWithPrimitive ImplicitShape::nearestIntersectionWithPrimitive(
        std::vector<IntersectionWithPrimitive> intersections) {
    float minTValue = INFINITY;
    float minIndex = 0;
    for (int i = 0; i < intersections.size(); i++) {
        float t = intersections[i].t;
        if (t >= EPSILON && t < minTValue) {
            minTValue = t;
            minIndex = i;
        }
    }
    if (minTValue == INFINITY) {
        // Return null intersection (t = -1)
        return IntersectionWithPrimitive();
    } else {
        return intersections[minIndex];
    }
}

/** Return the smallest non-negative value, or -1 if none exsits */
float ImplicitShape::nonNegativeMin(std::vector<float> values) {
    float min = INFINITY;
    for (int i = 0; i < values.size(); i++) {
        float val = values[i];
        if (val >= EPSILON && val < min) {
            min = val;
        }
    }
    if (min == INFINITY) {
        return -1;
    } else {
        return min;
    }
}

/**
 *  Solve a quadratic equation defined by coefficients a, b, and c.
 *  Returns 0, 1, or 2 real roots
 */
std::vector<float> ImplicitShape::solveQuadratic(float a, float b, float c) {
    std::vector<float> roots;
    float discriminant = b * b - 4 * a *c;
    if (discriminant > 0) {
        // Two real roots for positive discriminant
        roots.push_back((-b + sqrt(discriminant)) / (2*a));
        roots.push_back((-b - sqrt(discriminant)) / (2*a));
    } else if (abs(discriminant) < EPSILON) {
        // One real root for discriminant = 0
        roots.push_back((-b + sqrt(discriminant)) / (2*a));
    }
    return roots;
}

/**
 *  Checks whether given point is within a horizontal circle with radius 0.5 around the origin.
 *  Useful for calculating intersections with cylinder and cone caps.
 */
bool ImplicitShape::isWithinHorizontalCircle(glm::vec3 point) {
    float x = point.x;
    float z = point.z;
    return x * x + z * z < 0.25;
}
