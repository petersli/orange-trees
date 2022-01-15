#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include "CS123SceneData.h"
#include "LSystem.h"

const float pi = 3.14159265359;

// Base likelihood that a branching point has a fruit
const float baseFruitDensity = 0.4;
// Min/max branching levels for adding fruit
const int minFruitRecursiveDepth = 3;
const int maxFruitRecursiveDepth = 6;
// Min/max branching levels for adding leaves
const int minLeafRecursiveDepth = 2;
const int maxLeafRecursiveDepth = 9;
// Base amount of y-axis rotation for '+' symbol
const float thetaPlus = 0.5 * pi;
// Base amount of y-axis rotation for '-' symbol
const float thetaMinus = 0.3 * pi;
// Base amount of x-axis rotation
const float baseXRotation = 0.3;
// Amount to scale x, z size of each successive iteration
const float branchWidthDecay = 0.7;

class MeshGenerator
{
public:
    MeshGenerator();
    void generateTree();
    std::vector<CS123ScenePrimitive> getPrimitives();
    std::vector<glm::mat4> getTransformations();
    std::vector<CS123ScenePrimitive> getFruitPrimitives();
    std::vector<glm::mat4> getFruitTransformations();
private:
    std::unique_ptr<LSystem> m_lSystem;
    void initializeLSystem();
    void parseLSystem(std::string lSystemString);

    std::unique_ptr<CS123ScenePrimitive> m_trunk;
    glm::mat4 m_trunkPreTransform;
    void initializeTrunkPrimitive();

    std::unique_ptr<CS123ScenePrimitive> m_leaf;
    glm::mat4 m_leafPreTransform;
    glm::mat4 m_leafPostTransform;
    void initializeLeafPrimitive();

    std::unique_ptr<CS123ScenePrimitive> m_fruit;
    glm::mat4 m_fruitPreTransform;
    glm::mat4 m_fruitPostTransform;
    void initializeFruitPrimitive();

    void addPrimitive(CS123ScenePrimitive scenePrimitive, glm::mat4 transformation);
    std::vector<CS123ScenePrimitive> m_primitives;
    std::vector<glm::mat4> m_transformations;
    // We store fruits separately because they use a different shader for physics effects
    void addFruitPrimitive(CS123ScenePrimitive scenePrimitive, glm::mat4 transformation);
    std::vector<CS123ScenePrimitive> m_fruitPrimitives;
    std::vector<glm::mat4> m_fruitTransformations;

    float getYRotateAnglePlus();
    float getYRotateAngleMinus();
    float getXRotateAngle();
    float getBranchLength();
};

#endif // MESHGENERATOR_H
