#include "Cylinder.h"
#include "TriMesh.h"
#include <math.h>
#include <iostream>
#include "glm/gtx/string_cast.hpp"

Cylinder::Cylinder(int param1, int param2) :
    Shape(param1, param2),
    m_tessellator(nullptr),
    m_top(nullptr),
    m_bottom(nullptr)
{
    m_tessellator = std::make_unique<Tessellator>();
    m_top = std::make_unique<CircleBase>(m_param1, m_param2, true);
    m_bottom = std::make_unique<CircleBase>(m_param1, m_param2, false);
    Cylinder::initializeVertexData();
    initializeOpenGLShapeProperties();
}

Cylinder::~Cylinder()
{
}


std::vector<glm::vec3> Cylinder::makeSideGrid() {
    int height = m_param1 + 1.0f;
    int width = m_param2 + 1.0f;
    std::vector<glm::vec3> grid;
    grid.reserve(height * width);
    for (int row = 0; row < height; row++) {
        float y = 0.5f - static_cast<float>(row) / m_param1;
        for (int col = 0; col < width; col++) {
            float theta = 2.0f * PI * static_cast<float>(col) / m_param2;
            float x = -0.5f * cos(theta);
            float z = 0.5f * sin(theta);
            grid.push_back(glm::vec3(x, y, z));
        }
    }
    return grid;
}

void Cylinder::setSideNormals(std::vector<Triangle> &faces) {
    int width = m_param2 + 1.0f;
    for (int i = 0; i < faces.size(); i++) {
        Triangle &face = faces[i];
        for (int j = 0; j < 3; j++) {
            int col = face.vertexIndices[j] % width;
            float theta = 2.0f * PI * static_cast<float>(col) / m_param2;
            face.vertexNormals.push_back(glm::vec3(-cos(theta), 0.f, sin(theta)));
        }
    }
}

void Cylinder::initializeVertexData() {
    // Update params
    m_param2 = std::max(3, m_param2);
    m_top->setParam1(m_param1);
    m_top->setParam2(m_param2);
    m_bottom->setParam1(m_param1);
    m_bottom->setParam2(m_param2);

    int height = m_param1 + 1.0f;
    int width = m_param2 + 1.0f;

    // Build side of cylinder
    std::vector<glm::vec3> sideVertices = makeSideGrid();
    std::vector<Triangle> sideFaces = m_tessellator->tessellate(width, height);
    setSideNormals(sideFaces);
    TriMesh sideMesh = TriMesh(sideVertices, sideFaces);
    std::vector<float> sideVertexData = m_tessellator->processTriMesh(sideMesh);

    // Get top and bottom vertex data
    std::vector<float> topVertexData = m_top->getVertexData();
    std::vector<float> bottomVertexData = m_bottom->getVertexData();

    // Combine vertex data
    sideVertexData.insert(sideVertexData.end(), topVertexData.begin(), topVertexData.end());
    sideVertexData.insert(sideVertexData.end(), bottomVertexData.begin(), bottomVertexData.end());
    m_vertexData = sideVertexData;
}
