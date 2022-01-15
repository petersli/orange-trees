#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "trees/terrain.h"
#include <iostream>


using namespace CS123::GL;


SceneviewScene::SceneviewScene() :
    m_treeGenerator(nullptr),
    m_recursionDepth(0),
    m_cube(nullptr),
    m_sphere(nullptr),
    m_cone(nullptr),
    m_cylinder(nullptr),
    m_leaf(nullptr),
    m_fruit(nullptr),
    m_trunk(nullptr),
    m_fruit_index(0),
    m_shapesTessellated(false)
{

    m_implicitShape = std::make_unique<ImplicitShape>();
    m_implicitSphere = std::make_unique<ImplicitSphere>();

    loadPhongShader();
    loadTerrainShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
    tessellateShapes();
    initializeTreeScene();
}

SceneviewScene::~SceneviewScene()
{
}

/** Set up everything needed for the tree scene */
void SceneviewScene::initializeTreeScene() {

    // Generate terrain
    m_terrain = std::make_unique<Terrain>();
    std::vector<float> data = m_terrain->init();
    m_terrain->openGLShape = std::make_unique<OpenGLShape>(true);
    m_terrain->openGLShape->m_vertexData = data;
    m_terrain->openGLShape->initializeOpenGLShapeProperties();


    m_treeGenerator = std::make_unique<MeshGenerator>();
    defineLights();
    defineGlobalData();
    regenerateTree();
}

/** Get new tree from generator and set scene data accordingly */
void SceneviewScene::regenerateTree() {
    m_treeGenerator->generateTree();
    updateSceneFromTree();
}

/** Ensure scene primitives are updated to match the latest tree*/
void SceneviewScene::updateSceneFromTree() {
    m_primitives.clear();
    m_matrices.clear();
    m_fruitPhysics.clear();
    m_fruit_index = 0;
    std::vector<CS123ScenePrimitive> treePrimitives = m_treeGenerator->getPrimitives();
    std::vector<glm::mat4> treeTransformations = m_treeGenerator->getTransformations();

    // Adjust for terrain height
    glm::mat4 trunkAdj = glm::translate(glm::vec3(0, m_terrain->getHeightFromWorld(glm::vec3(0.0f))-0.25, 0));

    for (int i = 0; i < treePrimitives.size(); i++) {
        m_primitives.push_back(treePrimitives[i]);
        m_matrices.push_back(trunkAdj * treeTransformations[i]);
    }
    std::vector<CS123ScenePrimitive> fruitPrimitives = m_treeGenerator->getFruitPrimitives();
    std::vector<glm::mat4> fruitTransformations = m_treeGenerator->getFruitTransformations();
    m_fruitOffset = m_primitives.size();
    for (int i = 0; i < fruitPrimitives.size(); i++) {
        m_primitives.push_back(fruitPrimitives[i]);
        m_matrices.push_back(trunkAdj * fruitTransformations[i]);
        glm::vec3 start_pos = (trunkAdj * fruitTransformations[i] * glm::vec4(0, 0, 0, 1)).xyz();
                auto t = std::make_unique<FruitTransformation>(start_pos);
                m_fruitPhysics.push_back(std::move(t));
    }
}

/** Define the lights we need for our tree scene */
void SceneviewScene::defineLights() {
    m_lights.clear();
    CS123SceneLightData light;
    light.type = LightType::LIGHT_DIRECTIONAL;
    light.dir = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));
    light.color.r = light.color.g = light.color.b = 1;
    light.id = 0;
    m_lights.push_back(light);
}

/** Define the global data we need for our tree scene */
void SceneviewScene::defineGlobalData() {
    m_globalData.ka = 1.0f;
    m_globalData.kd = 1.0f;
    m_globalData.ks = 1.0f;
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadTerrainShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/shader-terr.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/shader-terr.frag");
    m_terrainShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::render(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




    m_phongShader->bind();
    setGlobalData();
    setSceneUniforms(context);
    setLightsInShader();
    renderGeometry();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

    // Render terrain
    m_terrainShader->bind();
    m_terrainShader->setUniform("model", glm::mat4(1.f));
    m_terrainShader->setUniform("view", camera->getViewMatrix());
    m_terrainShader->setUniform("projection", camera->getProjectionMatrix());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_terrain->openGLShape->draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_terrainShader->unbind();
}

void SceneviewScene::setGlobalData(){
    // [TODO] pass global data to shader.vert using m_phongShader
    m_phongShader->setUniform("ka", m_globalData.ka);
    m_phongShader->setUniform("kd", m_globalData.kd);
    m_phongShader->setUniform("ks", m_globalData.ks);
}

void SceneviewScene::setSceneUniforms(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("isShapeScene", false);
    m_phongShader->setUniform("p" , camera->getProjectionMatrix());
    m_phongShader->setUniform("v", camera->getViewMatrix());
}

void SceneviewScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

void SceneviewScene::setLightsInShader()
{
    for (CS123SceneLightData &light : m_lights) {
        m_phongShader->setLight(light);
    }
}

/**
 *  Tessellate scene shapes and store them for later rendering
 */
void SceneviewScene::tessellateShapes() {
    m_cube = std::make_unique<Cube>(shapesParam1, shapesParam2);
    m_sphere = std::make_unique<Sphere>(shapesParam1, shapesParam2);
    m_cone = std::make_unique<Cone>(shapesParam1, shapesParam2);
    m_cylinder = std::make_unique<Cylinder>(shapesParam1, shapesParam2);
    m_leaf = std::make_unique<Leaf>();
    m_fruit = std::make_unique<Fruit>(shapesParam1, shapesParam2);
    m_trunk = std::make_unique<Trunk>(shapesParam1, shapesParam2);
}

void SceneviewScene::renderGeometry() {


    // Tessellate shapes and terrain on first render
    if (!m_shapesTessellated) {
        tessellateShapes();
        m_shapesTessellated = true;
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (int i = 0; i < m_primitives.size(); i++) {
        glm::mat4 ctm = m_matrices[i];
        // Set model (object -> world) matrix uniform on GPU
        m_phongShader->setUniform("m", ctm);
        // Set object material
        m_phongShader->applyMaterial(m_primitives[i].material);
        // Draw the shape
        CS123ScenePrimitive primitive = m_primitives[i];
        if (primitive.type == PrimitiveType::PRIMITIVE_TRUNK) {
            m_trunk->draw();
        } else if (primitive.type == PrimitiveType::PRIMITIVE_LEAF) {
            m_leaf->draw();
        } else if (primitive.type == PrimitiveType::PRIMITIVE_FRUIT) {
            int cur_idx = i - m_fruitOffset;

            glm::mat4 new_trans = m_fruitPhysics[cur_idx]->updatePosition(m_terrain);
            ctm = new_trans * ctm;
            m_matrices[i] = ctm;
            m_phongShader->setUniform("m", ctm);
            m_fruit->draw();
        } else if (primitive.type == PrimitiveType::PRIMITIVE_CONE) {
            m_cone->draw();
        } else if (primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
            m_cylinder->draw();
        } else if (primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
            m_cube->draw();
        } else {
            m_sphere->draw();
        }
    }
}

void SceneviewScene::keyPressed(SupportCanvas3D *canvas, CS123SceneCameraData *camera, int col, int row){

    std::cout << "Col: " << col << " Row: " << row << std::endl;

    pickFruit(canvas, camera, col, row);

    /*if (m_fruitPhysics.size() > 0 && m_fruit_index < m_fruitPhysics.size()){
        m_fruitPhysics[m_fruit_index]->m_isFalling = true;
        m_fruit_index++;
    }*/

}

glm::mat4 getCameraMatrix(CS123SceneCameraData *camera) {
    glm::vec4 u;    // camera space vector to the "right"
    glm::vec4 v;    // camera space vector from eye to top of camera
    glm::vec4 w;    // camera space vector opposite look vector
    w = glm::normalize(-camera->look);
    v = glm::normalize(camera->up - glm::dot(camera->up, w) * w);
    u = glm::vec4(glm::cross(glm::vec3(v), glm::vec3(w)), 0.f);
    glm::mat4 translateMatrix = glm::translate(glm::vec3(camera->pos));
    glm::mat4 rotateMatrix = glm::transpose(glm::mat4(u, v, w, glm::vec4(0.f, 0.f, 0.f, 1.0f)));
    return rotateMatrix * translateMatrix;
}

glm::vec3 getRayCameraDirection(float widthAngle, float heightAngle,
                                          int width, int height, int col, int row) {
    float x = (col + 0.5f) / width - 0.5f;
    float y = 0.5f - (row + 0.5f) / height;
    float U = 2 * .1 * tan(widthAngle / 2.0f);
    float V = 2 * .1 * tan(heightAngle / 2.0f);
    glm::vec3 viewPlanePoint = glm::vec3(U * x, V * y, -.1);
    return glm::normalize(viewPlanePoint);
}

IntersectionWithPrimitive SceneviewScene::rayObjectIntersection(Ray ray) {
    std::vector<IntersectionWithPrimitive> intersections;
    for (int i = m_fruitOffset; i < m_primitives.size(); i++) {
        // Cumulative transformation matrix
        glm::mat4 ctm = m_matrices[i];
        glm::mat4 inverseCtm = glm::inverse(ctm);
        // Convert to object space using inverse of the CTM
        glm::vec3 objectSpaceDirection = glm::mat3(inverseCtm) * ray.direction;
        glm::vec3 objectSpaceEye = glm::vec3(inverseCtm * glm::vec4(ray.startPoint, 1.0f));
        Ray objectSpaceRay = Ray(objectSpaceEye, objectSpaceDirection);
        // Check for intersection
        CS123ScenePrimitive primitive = m_primitives[i];
        if (primitive.type == PrimitiveType::PRIMITIVE_FRUIT) {
            Intersection intersection = m_implicitSphere->intersect(objectSpaceRay);
            UV uv = m_implicitSphere->mapToUV(intersection.objectSpacePos);
            intersections.push_back(IntersectionWithPrimitive(intersection, i, uv));
        }
    }
    IntersectionWithPrimitive nearestIntersection =
            m_implicitShape->nearestIntersectionWithPrimitive(intersections);
    return nearestIntersection;
}

/**
 *  Trace a ray, returning the color at the intersection point, or black
 *  if there is no intersection. Recursively computes inter-object reflection by
 *  tracing reflected rays until max recursion depth is reached or no intersection
 *  is found.
 */
int SceneviewScene::traceRay(Ray ray) {
    // Find and store the nearest intersection with a primitive object
    IntersectionWithPrimitive nearestIntersection = rayObjectIntersection(ray);
    // If no valid intersection, return black
    if (nearestIntersection.t < EPSILON) {
        return -1;
    }

    int primitiveIndex = nearestIntersection.primitiveIndex;

    return primitiveIndex;

}

/*
 * Fruit intersection code
 */
void SceneviewScene::pickFruit(SupportCanvas3D *canvas, CS123SceneCameraData *camera, int col, int row) {


    // Convert degrees to radians
    float aspectRatio = static_cast<float>(canvas->width()) / canvas->height();
    //float aspectRatio = camera->aspectRatio;
    float heightAngle = camera->heightAngle * PI / 180.0f;
    float widthAngle = heightAngle * aspectRatio;

    // Set up camera transformations
    glm::vec3 cameraPos = glm::vec3(camera->pos);
    glm::mat4 worldToCameraSpace = getCameraMatrix(camera);
    glm::mat4 cameraToWorldSpace = glm::inverse(worldToCameraSpace);
    glm::mat3 cameraToWorldSpace3 = glm::mat3(cameraToWorldSpace);

    glm::vec3 cameraSpaceDirection = getRayCameraDirection(
                widthAngle, heightAngle, canvas->width(), canvas->height(), col, row);
    // Transform to world space and trace ray
    glm::vec3 worldSpaceDirection = cameraToWorldSpace3 * cameraSpaceDirection;
    int found_fruit = traceRay(Ray(cameraPos, worldSpaceDirection));

    if (found_fruit > -1){
        if (!m_fruitPhysics[found_fruit - m_fruitOffset]->m_isRolling){
            m_fruitPhysics[found_fruit - m_fruitOffset]->m_isFalling = true;
        }
    }

}

void SceneviewScene::dropFruit(){

    for (int i = m_fruit_index; i < m_fruitPhysics.size(); i++){

        if (!m_fruitPhysics[i]->m_isFalling && !m_fruitPhysics[i]->m_isRolling){
            m_fruitPhysics[i]->m_isFalling = true;
            m_fruit_index = i + 1;
            return;
        }
    }
}



void SceneviewScene::settingsChanged() {
    if (m_recursionDepth != settings.recursionDepth) {
        regenerateTree();
        m_recursionDepth = settings.recursionDepth;
    }
    if (m_fruitDensity != settings.fruitDensity) {
        regenerateTree();
        m_fruitDensity = settings.fruitDensity;
    }
    if (m_leafDensity != settings.leafDensity) {
        regenerateTree();
        m_leafDensity = settings.leafDensity;
    }
    if (m_branchStochasticity != settings.branchStochasticity) {
        regenerateTree();
        m_branchStochasticity = settings.branchStochasticity;
    }
}

