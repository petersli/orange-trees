#include "ImplicitCube.h"

ImplicitCube::ImplicitCube()
{

}

/** Compute intersection with cube that has range [-0.5, 0.5] in all dimensions */
Intersection ImplicitCube::intersect(Ray ray) {
    std::vector<Intersection> intersections;

    // Check for intersection with top of cube
    Plane topPlane = Plane(glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f, 1.0f, 0.f));
    float tTop = intersectPlane(ray, topPlane);
    glm::vec3 topIntersection = pointAlongRay(ray, tTop);
    if (isWithinXZSquare(topIntersection)) {
        glm::vec3 normal = glm::vec3(0.f, 1.0f, 0.f);
        intersections.push_back(Intersection(tTop, topIntersection, normal));
    }

    // Check for intersection with bottom of cube
    Plane bottomPlane = Plane(glm::vec3(0.f, -0.5f, 0.f), glm::vec3(0.f, -1.0f, 0.f));
    float tBottom = intersectPlane(ray, bottomPlane);
    glm::vec3 bottomIntersection = pointAlongRay(ray, tBottom);
    if (isWithinXZSquare(bottomIntersection)) {
        glm::vec3 normal = glm::vec3(0.f, -1.0f, 0.f);
        intersections.push_back(Intersection(tBottom, bottomIntersection, normal));
    }

    // Check for intersection with side in positive X plane
    Plane posXPlane = Plane(glm::vec3(0.5f, 0.f, 0.f), glm::vec3(1.0f, 0.f, 0.f));
    float tPosX = intersectPlane(ray, posXPlane);
    glm::vec3 posXIntersection = pointAlongRay(ray, tPosX);
    if (isWithinYZSquare(posXIntersection)) {
        glm::vec3 normal = glm::vec3(1.0f, 0.f, 0.f);
        intersections.push_back(Intersection(tPosX, posXIntersection, normal));
    }

    // Check for intersection with side in negative X plane
    Plane negXPlane = Plane(glm::vec3(-0.5f, 0.f, 0.f), glm::vec3(-1.0f, 0.f, 0.f));
    float tNegX = intersectPlane(ray, negXPlane);
    glm::vec3 negXIntersection = pointAlongRay(ray, tNegX);
    if (isWithinYZSquare(negXIntersection)) {
        glm::vec3 normal = glm::vec3(-1.0f, 0.f, 0.f);
        intersections.push_back(Intersection(tNegX, negXIntersection, normal));
    }

    // Check for intersection with side in positive Z plane
    Plane posZPlane = Plane(glm::vec3(0.f, 0.f, 0.5f), glm::vec3(0.f, 0.f, 1.0f));
    float tPosZ = intersectPlane(ray, posZPlane);
    glm::vec3 posZIntersection = pointAlongRay(ray, tPosZ);
    if (isWithinXYSquare(posZIntersection)) {
        glm::vec3 normal = glm::vec3(0.f, 0.f, 1.0f);
        intersections.push_back(Intersection(tPosZ, posZIntersection, normal));
    }

    // Check for intersection with side in negative Z plane
    Plane negZPlane = Plane(glm::vec3(0.f, 0.f, -0.5f), glm::vec3(0.f, 0.f, -1.0f));
    float tNegZ = intersectPlane(ray, negZPlane);
    glm::vec3 negZIntersection = pointAlongRay(ray, tNegZ);
    if (isWithinXYSquare(negZIntersection)) {
        glm::vec3 normal = glm::vec3(0.f, 0.f, -1.0f);
        intersections.push_back(Intersection(tNegZ, negZIntersection, normal));
    }

    return m_implicitShape->nearestIntersection(intersections);
}

/** Whether point has -0.5 <= x, z <= 0.5 */
bool ImplicitCube::isWithinXZSquare(glm::vec3 point) {
    return abs(point.x) <= 0.5 && abs(point.z) <= 0.5;
}

/** Whether point has -0.5 <= x, y <= 0.5 */
bool ImplicitCube::isWithinXYSquare(glm::vec3 point) {
    return abs(point.x) <= 0.5 && abs(point.y) <= 0.5;
}

/** Whether point has -0.5 <= y, z <= 0.5 */
bool ImplicitCube::isWithinYZSquare(glm::vec3 point) {
    return abs(point.y) <= 0.5 && abs(point.z) <= 0.5;
}

/** Map a position on the cube surface to a UV coord for texture mapping */
UV ImplicitCube::mapToUV(glm::vec3 pos) {
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    // For top face, we just need to offset by the radius
    if (abs(y - radius) < EPSILON) {
        return UV(x + radius, z + radius);
    }
    // For bottom face, we reflect vertically
    if (abs(y + radius) < EPSILON) {
        return UV(x + radius, -z + radius);
    }
    // Positive X face
    if (abs(x + radius) < EPSILON) {
        return UV(z + radius, -y + radius);
    }
    // Negative X face
    if (abs(x - radius) < EPSILON) {
        return UV(-z + radius, -y + radius);
    }
    // Positive Z face
    if (abs(z + radius) < EPSILON) {
        return UV(-x + radius, -y + radius);
    }
    // Negative Z face
    if (abs(z - radius) < EPSILON) {
        return UV(x + radius, -y + radius);
    }
    // Not a point on cube surface
    return UV(0, 0);
}



