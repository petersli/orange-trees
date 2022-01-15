#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>


Scene::Scene()
{
}

Scene::Scene(Scene &scene)
{
    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.
    m_globalData = scene.m_globalData;
    m_lights = scene.m_lights;
    m_primitives = scene.m_primitives;
    m_matrices = scene.m_matrices;
    m_textures = scene.m_textures;
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()
    // Add global data
    CS123SceneGlobalData globalData;
    parser->getGlobalData(globalData);
    sceneToFill->setGlobal(globalData);

    // Add light data
    int numLights = parser->getNumLights();
    for (int i = 0; i < numLights; i++) {
        CS123SceneLightData light;
        parser->getLightData(i, light);
        sceneToFill->addLight(light);
    }

    // Add primitive data
    CS123SceneNode *root = parser->getRootNode();
    glm::mat4 cumulativeMatrix = glm::mat4(1.0f);
    sceneToFill->traverseSceneGraph(root, cumulativeMatrix);
}

/** Recursively add primitives to scene */
void Scene::traverseSceneGraph(CS123SceneNode *node, glm::mat4 cumulativeMatrix) {
    std::vector<CS123SceneTransformation *> transformations = node->transformations;
    // Accumulate transformation matrices
    for (int i = 0; i < transformations.size(); i++) {
        glm::mat4 childMatrix = getMatrixForTransformation(*transformations[i]);
        cumulativeMatrix = cumulativeMatrix * childMatrix;
    }
    // For primitives, add to scene, along with transformation matrix and texture
    std::vector<CS123ScenePrimitive *> primitives = node->primitives;
    for (int i = 0; i < primitives.size(); i++) {
        CS123ScenePrimitive primitive = *primitives[i];
        CS123SceneFileMap textureMap = primitive.material.textureMap;
        QImage *texture;
        if (textureMap.isUsed) {
            QString filename = QString::fromStdString(textureMap.filename);
            texture = new QImage(filename);
        } else {
            // Null image
            texture = new QImage();
        }
        addPrimitive(primitive, cumulativeMatrix, *texture);
    }
    // For node with non-primitive children
    std::vector<CS123SceneNode *> children = node->children;
    for (int i = 0; i < children.size(); i++) {
        traverseSceneGraph(children[i], cumulativeMatrix);
    }
}

glm::mat4 Scene::getMatrixForTransformation(const CS123SceneTransformation &transform) {
    if (transform.type == TRANSFORMATION_SCALE) {
        return glm::scale(glm::mat4(1.0f), transform.scale);
    } else if (transform.type == TRANSFORMATION_ROTATE) {
        return glm::rotate(glm::mat4(1.0f), transform.angle, transform.rotate);
    } else if (transform.type == TRANSFORMATION_TRANSLATE) {
        return glm::translate(glm::mat4(1.0f), transform.translate);
    } else {
        // It's a custom matrix transformation
        return transform.matrix;
    }
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive,
                         const glm::mat4x4 &matrix, const QImage &texture) {
    m_primitives.push_back(scenePrimitive);
    m_matrices.push_back(matrix);
    m_textures.push_back(texture);
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_lights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_globalData = global;
}
