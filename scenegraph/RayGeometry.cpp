#include "RayGeometry.h"

/** Return the point along a ray defined by the parameter t */
glm::vec3 pointAlongRay(Ray ray, float t) {
    return ray.startPoint + t * ray.direction;
}


/** Return t-value for intersection with a plane */
float intersectPlane(Ray ray, Plane plane) {
    float topTerm = glm::dot(plane.normal, plane.point) - glm::dot(plane.normal, ray.startPoint);
    float bottomTerm = glm::dot(plane.normal, ray.direction);
    return topTerm / bottomTerm;
}

