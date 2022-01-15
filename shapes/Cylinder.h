#ifndef CYLINDER_H
#define CYLINDER_H

#define PI 3.14159265f

#include "Tessellator.h"
#include "TriMesh.h"
#include "Shape.h"
#include "CircleBase.h"

/** Cylinder bounded by a 1x1x1 cube centered at the origin */
class Cylinder : public Shape
{
public:
    Cylinder(int param1, int param2);
    ~Cylinder();

private:
    void initializeVertexData() override;
    std::unique_ptr<Tessellator> m_tessellator;
    std::unique_ptr<CircleBase> m_top;
    std::unique_ptr<CircleBase> m_bottom;
    std::vector<glm::vec3> makeSideGrid();
    void setSideNormals(std::vector<Triangle> &faces);
};


#endif // CYLINDER_H
