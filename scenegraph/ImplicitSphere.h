#ifndef IMPLICITSPHERE_H
#define IMPLICITSPHERE_H

#include "ImplicitShape.h"


class ImplicitSphere
{
public:
    ImplicitSphere();
    Intersection intersect(Ray ray);
    UV mapToUV(glm::vec3 pos);
private:
    std::unique_ptr<ImplicitShape> m_implicitShape;
};

#endif // IMPLICITSPHERE_H
