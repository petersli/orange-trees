#include "ImplicitCone.h"

ImplicitCone::ImplicitCone()
{
    m_implicitShape = std::make_unique<ImplicitShape>();
}


/**
 *  Compute interseciton with cone primitive centered around origin
 *  (base at y = -0.5, tip at y = 0.5, radius 0.5 at base)
 *  Returns t-value of intersection, or -1 if none exists.
 */
Intersection ImplicitCone::intersect(Ray ray) {
    std::vector<Intersection> intersections;

    // Check for intersection with infinite cone
    std::vector<float> infiniteConeTValues = intersectInfiniteCone(ray);
    for (int i = 0; i < infiniteConeTValues.size(); i++) {
        float t = infiniteConeTValues[i];
        glm::vec3 intersection = pointAlongRay(ray, t);
        if (abs(intersection.y - 0.5f) < EPSILON) {
            glm::vec3 tipNormal = glm::vec3(0.f, 1.0f, 0.f);
            intersections.push_back(Intersection(t, intersection, tipNormal));
        }
        else if (intersection.y >= -0.5 && intersection.y < 0.5) {
            glm::vec3 normal = getConeSideNormal(intersection);
            intersections.push_back(Intersection(t, intersection, normal));
        }
    }

    // Check for intersection with bottom cap
    Plane bottomPlane = Plane(glm::vec3(0.f, -0.5f, 0.f), glm::vec3(0.f, -1.0f, 0.f));
    float tBottom = intersectPlane(ray, bottomPlane);
    glm::vec3 bottomIntersection = pointAlongRay(ray, tBottom);
    if (m_implicitShape->isWithinHorizontalCircle(bottomIntersection)) {
        glm::vec3 normal = glm::vec3(0.f, -1.0f, 0.f);
        intersections.push_back(Intersection(tBottom, bottomIntersection, normal));
    }

    return m_implicitShape->nearestIntersection(intersections);
}


/**
 *  Return t-values for up to 2 intersections with an infinite cone
 *  (centered around the origin, radius 0.5 at y = -0.5, tip at y = 0.5)
 */
std::vector<float> ImplicitCone::intersectInfiniteCone(Ray ray) {
    glm::vec3 direction = ray.direction;
    glm::vec3 eye = ray.startPoint;

    float a = direction.x * direction.x + direction.z * direction.z
            - 0.25 * direction.y * direction.y;
    float b = 2 * (eye.x * direction.x + eye.z * direction.z) - 0.5 * eye.y * direction.y
            + 0.25 * direction.y;
    float c = eye.x * eye.x + eye.z * eye.z - 0.25 * eye.y * eye.y + 0.25 * eye.y - 0.0625;
    return m_implicitShape->solveQuadratic(a, b, c);
}

/** Get normal from the cone side based on the gradient of the cone's implicit equation */
glm::vec3 ImplicitCone::getConeSideNormal(glm::vec3 point) {
    float x = point.x;
    float y = point.y;
    float z = point.z;
    glm::vec3 normal = glm::vec3(2 * x, 0.25f - 0.5f * y, 2 * z);
    return glm::normalize(normal);
}


/** Map a position on the cone surface to a UV coord for texture mapping */
UV ImplicitCone::mapToUV(glm::vec3 pos) {
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    // For bottom base, we reflect vertically
    if (abs(y + radius) < EPSILON) {
        return UV(x + radius, -z + radius);
    }
    // For side of cone, we use theta and y
    float theta = atan2(z, x);
    float u = 1 - theta / (2 * PI);
    float v = radius - y;
    return UV(u, v);
}
