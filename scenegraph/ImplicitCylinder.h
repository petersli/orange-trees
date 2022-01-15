#ifndef IMPLICITCYLINDER_H
#define IMPLICITCYLINDER_H

#include "ImplicitShape.h"


class ImplicitCylinder
{
public:
    ImplicitCylinder();
    Intersection intersect(Ray ray);
    UV mapToUV(glm::vec3 pos);
private:
    std::unique_ptr<ImplicitShape> m_implicitShape;
    std::vector<float> intersectInfiniteCylinder(Ray ray);
};

#endif // IMPLICITCYLINDER_H
