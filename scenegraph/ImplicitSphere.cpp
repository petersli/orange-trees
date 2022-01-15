#include "ImplicitSphere.h"

ImplicitSphere::ImplicitSphere()
{
    m_implicitShape = std::make_unique<ImplicitShape>();
}


/** Intersect a sphere with radius 0.5 centered at the origin */
Intersection ImplicitSphere::intersect(Ray ray) {
    std::vector<float> tValues;

    glm::vec3 direction = ray.direction;
    glm::vec3 eye = ray.startPoint;

    float a = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
    float b = 2 * (eye.x * direction.x + eye.y * direction.y + eye.z * direction.z);
    float c = eye.x * eye.x + eye.y * eye.y + eye.z * eye.z - 0.25f;
    tValues = m_implicitShape->solveQuadratic(a, b, c);

    float nearestTValue =  m_implicitShape->nonNegativeMin(tValues);
    if (nearestTValue > 0) {
        glm::vec3 intersectionPoint = pointAlongRay(ray, nearestTValue);
        // Normal for sphere is same as normalized point
        glm::vec3 normal = glm::normalize(intersectionPoint);
        return Intersection(nearestTValue, intersectionPoint, normal);
    } else {
        // Return null intersection (t = -1)
        return Intersection();
    }
}

/** Map a position on the sphere surface to a UV coord for texture mapping */
UV ImplicitSphere::mapToUV(glm::vec3 pos) {
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    float theta = atan2(z, x);
    float phi = asin(y / radius) + PI / 2;
    float u = 1 - theta / (2 * PI);
    float v = 1 - phi / PI;
    return UV(u, v);
}

