#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H

#include "ImplicitShape.h"


class ImplicitCone
{
public:
    ImplicitCone();
    Intersection intersect(Ray ray);
    UV mapToUV(glm::vec3 pos);
private:
    std::unique_ptr<ImplicitShape> m_implicitShape;
    std::vector<float> intersectInfiniteCone(Ray ray);
    glm::vec3 getConeSideNormal(glm::vec3 point);
};

#endif // IMPLICITCONE_H
