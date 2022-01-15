#include "ImplicitCylinder.h"

ImplicitCylinder::ImplicitCylinder()
{
    m_implicitShape = std::make_unique<ImplicitShape>();
}


/**
 *  Compute interseciton with vertical cylinder primitive
 *  (radius 0.5, centered at origin with caps at y = 0.5, -0.5)
 *  Returns t-value of intersection, or -1 if none exists.
 */
Intersection ImplicitCylinder::intersect(Ray ray) {
    std::vector<Intersection> intersections;

    // Check for intersection with infinite cylinder
    std::vector<float> infiniteCylinderTValues = intersectInfiniteCylinder(ray);
    for (int i = 0; i < infiniteCylinderTValues.size(); i++) {
        float t = infiniteCylinderTValues[i];
        glm::vec3 intersection = pointAlongRay(ray, t);
        if (intersection.y >= -0.5 && intersection.y <= 0.5) {
            glm::vec3 normal = glm::normalize(glm::vec3(intersection.x, 0.f, intersection.z));
            intersections.push_back(Intersection(t, intersection, normal));
        }
    }

    // Check for intersection with top cap
    Plane topPlane = Plane(glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f, 1.0f, 0.f));
    float tTop = intersectPlane(ray, topPlane);
    glm::vec3 topIntersection = pointAlongRay(ray, tTop);
    if (m_implicitShape->isWithinHorizontalCircle(topIntersection)) {
        glm::vec3 normal = glm::vec3(0, 1.0f, 0);
        intersections.push_back(Intersection(tTop, topIntersection, normal));
    }

    // Check for intersection with bottom cap
    Plane bottomPlane = Plane(glm::vec3(0.f, -0.5f, 0.f), glm::vec3(0.f, -1.0f, 0.f));
    float tBottom = intersectPlane(ray, bottomPlane);
    glm::vec3 bottomIntersection = pointAlongRay(ray, tBottom);
    if (m_implicitShape->isWithinHorizontalCircle(bottomIntersection)) {
        glm::vec3 normal = glm::vec3(0, -1.0f, 0);
        intersections.push_back(Intersection(tBottom, bottomIntersection, normal));
    }

    return m_implicitShape->nearestIntersection(intersections);
}

/**
 *  Return t-values for up to 2 intersections with an infinite vertical cylinder
 *  (radius 0.5, centered around the origin)
 */
std::vector<float> ImplicitCylinder::intersectInfiniteCylinder(Ray ray) {
    glm::vec3 direction = ray.direction;
    glm::vec3 eye = ray.startPoint;

    float a = direction.x * direction.x + direction.z * direction.z;
    float b = 2 * (eye.x * direction.x + eye.z * direction.z);
    float c = eye.x * eye.x + eye.z * eye.z - 0.25f;
    return m_implicitShape->solveQuadratic(a, b, c);
}

/** Map a position on the cylinder surface to a UV coord for texture mapping */
UV ImplicitCylinder::mapToUV(glm::vec3 pos) {
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    // For top base, we just need to offset by the radius
    if (abs(y - radius) < EPSILON) {
        return UV(x + radius, z + radius);
    }
    // For bottom base, we reflect vertically
    if (abs(y + radius) < EPSILON) {
        return UV(x + radius, -z + radius);
    }
    // For side of cylinder, we use theta and y
    float theta = atan2(z, x);
    float u = 1 - theta / (2 * PI);
    float v = radius - y;
    return UV(u, v);
}
