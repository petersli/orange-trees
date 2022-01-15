#ifndef IMPLICITCUBE_H
#define IMPLICITCUBE_H

#include "ImplicitShape.h"


class ImplicitCube
{
public:
    ImplicitCube();
    Intersection intersect(Ray ray);
    UV mapToUV(glm::vec3 pos);
private:
    std::unique_ptr<ImplicitShape> m_implicitShape;
    bool isWithinXZSquare(glm::vec3 point);
    bool isWithinXYSquare(glm::vec3 point);
    bool isWithinYZSquare(glm::vec3 point);
};

#endif // IMPLICITCUBE_H
