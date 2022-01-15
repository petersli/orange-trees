#ifndef SCENEVIEWSCENE_H
#define SCENEVIEWSCENE_H

#include "OpenGLScene.h"
#include "shapes/Shape.h"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "shapes/Cone.h"
#include "shapes/Cylinder.h"
#include "shapes/Leaf.h"
#include "shapes/Fruit.h"
#include "shapes/Trunk.h"
#include "trees/MeshGenerator.h"
#include "trees/terrain.h"
#include "trees/FruitTransformation.h"
#include <QTime>
#include <QTimer>
#include "RayGeometry.h"
#include "scenegraph/ImplicitSphere.h"
#include "scenegraph/ImplicitShape.h"

#include <memory>

// Level of tessellation detail for Sceneview objects
const int shapesParam1 = 10;
const int shapesParam2 = 10;

namespace CS123 { namespace GL {

    class Shader;
    class CS123Shader;
    class Texture2D;
}}

/**
 *
 * @class SceneviewScene
 *
 * A complex scene consisting of multiple objects. Students will implement this class in the
 * Sceneview assignment.
 *
 * Here you will implement your scene graph. The structure is up to you - feel free to create new
 * classes and data structures as you see fit. We are providing this SceneviewScene class for you
 * to use as a stencil if you like.
 *
 * Keep in mind that you'll also be rendering entire scenes in the next two assignments, Intersect
 * and Ray. The difference between this assignment and those that follow is here, we are using
 * OpenGL to do the rendering. In Intersect and Ray, you will be responsible for that.
 */
class SceneviewScene : public OpenGLScene {
public:
    SceneviewScene();
    virtual ~SceneviewScene();

    virtual void render(SupportCanvas3D *context) override;
    virtual void settingsChanged() override;

    // Use this method to set an internal selection, based on the (x, y) position of the mouse
    // pointer.  This will be used during the "modeler" lab, so don't worry about it for now.
    void setSelection(int x, int y);

    void regenerateTree();
    void keyPressed(SupportCanvas3D *canvas, CS123SceneCameraData *camera, int x, int y) override;
    void dropFruit();

private:
    std::unique_ptr<MeshGenerator> m_treeGenerator;
    void updateSceneFromTree();    
    void initializeTreeScene();
    void defineLights();
    void defineGlobalData();
    int m_recursionDepth;
    float m_fruitDensity;
    float m_leafDensity;
    float m_branchStochasticity;

    void loadPhongShader();
    void loadTerrainShader();
    void loadWireframeShader();
    void loadNormalsShader();
    void loadNormalsArrowShader();

    void setGlobalData();
    void setSceneUniforms(SupportCanvas3D *context);
    void setMatrixUniforms(CS123::GL::Shader *shader, SupportCanvas3D *context);
    void setLightsInShader();
    void renderGeometry();
    void tessellateShapes();

    IntersectionWithPrimitive rayObjectIntersection(Ray ray);
    int traceRay(Ray ray);
    void pickFruit(SupportCanvas3D *canvas, CS123SceneCameraData *camera, int col, int row);

    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;
    std::unique_ptr<CS123::GL::Shader> m_wireframeShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;
    std::unique_ptr<CS123::GL::Shader> m_terrainShader;

    std::unique_ptr<Cube> m_cube;
    std::unique_ptr<Sphere> m_sphere;
    std::unique_ptr<Cone> m_cone;
    std::unique_ptr<Cylinder> m_cylinder;
    std::unique_ptr<Leaf> m_leaf;
    std::unique_ptr<Fruit> m_fruit;
    std::unique_ptr<Trunk> m_trunk;
    std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<ImplicitShape> m_implicitShape;
    std::unique_ptr<ImplicitSphere> m_implicitSphere;

    std::vector<std::unique_ptr<FruitTransformation>> m_fruitPhysics;
    int m_fruitOffset;
    int m_fruit_index;

    bool m_shapesTessellated;
};

#endif // SCENEVIEWSCENE_H
