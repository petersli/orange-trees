#ifndef RAYGEOMETRY_H
#define RAYGEOMETRY_H
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#define PI 3.14159265
#define EPSILON 0.0001

/**
 *  Our implicit cube, sphere, cone, and cylinder are bounded
 *  by [-radius, radius] in all dimensions
 */
const float radius = 0.5;

/**
 *  A ray to trace, made up of a start point and direction vector
 */
struct Ray {
    glm::vec3 startPoint;
    glm::vec3 direction;
    Ray(glm::vec3 startPoint, glm::vec3 direction) :
        startPoint(startPoint),
        direction(direction)
    {
    }
};

/**
 *  Definition of a plane based on a point in the plane and
 *  a vector normal to the plane
 */
struct Plane {
    glm::vec3 point;
    glm::vec3 normal;
    Plane(glm::vec3 point, glm::vec3 normal) :
        point(point),
        normal(normal)
    {
    }
};

/** UV coordinate in the unit square, used for texture mapping */
struct UV {
    float u;
    float v;
    UV(float u, float v) :
        u(u),
        v(v)
    {
    }
};

/**
 *  An object-ray intersection, including the t-value (so we can compute nearest intersection),
 *  point of intersection, and the object-space normal
 */
struct Intersection {
    float t;
    glm::vec3 objectSpacePos;
    glm::vec3 objectSpaceNormal;

    Intersection(float t, glm::vec3 objectSpacePos, glm::vec3 objectSpaceNormal) :
        t(t),
        objectSpacePos(objectSpacePos),
        objectSpaceNormal(objectSpaceNormal)
    {
    }

    // For null intersections
    Intersection() :
        t(-1),
        objectSpacePos(glm::vec3(0.f)),
        objectSpaceNormal(glm::vec3(0.f))
    {
    }
};

/**
 *  An object-ray intersection that includes the index of the intersected primitive.
 *  Otherwise, contains the same data as Intersection.
 */
struct IntersectionWithPrimitive {
    float t;
    glm::vec3 objectSpacePos;
    glm::vec3 objectSpaceNormal;
    int primitiveIndex;
    UV uv;

    IntersectionWithPrimitive(float t, glm::vec3 objectSpacePos,
                              glm::vec3 objectSpaceNormal, int primitiveIndex, UV uv) :
        t(t),
        objectSpacePos(objectSpacePos),
        objectSpaceNormal(objectSpaceNormal),
        primitiveIndex(primitiveIndex),
        uv(uv)
    {
    }

    IntersectionWithPrimitive(Intersection intersection, int primitiveIndex, UV uv) :
        t(intersection.t),
        objectSpacePos(intersection.objectSpacePos),
        objectSpaceNormal(intersection.objectSpaceNormal),
        primitiveIndex(primitiveIndex),
        uv(uv)
    {
    }

    // For null intersections
    IntersectionWithPrimitive() :
        t(-1),
        objectSpacePos(glm::vec3(0.f)),
        objectSpaceNormal(glm::vec3(0.f)),
        primitiveIndex(0),
        uv(0, 0)
    {
    }
};

glm::vec3 pointAlongRay(Ray ray, float t);
float intersectPlane(Ray ray, Plane plane);

#endif // RAYGEOMETRY_H
