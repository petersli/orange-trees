#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"
#include "SupportCanvas3D.h"
#include "QImage"

class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

    virtual void keyPressed(SupportCanvas3D *canvas, CS123SceneCameraData *camera, int col, int row)   {}

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive,
                              const glm::mat4x4 &matrix, const QImage &texture);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    // Traverse the scene graph, adding primitives and transformations
    void traverseSceneGraph(CS123SceneNode *node, glm::mat4 cumulativeMatrix);

    glm::mat4 getMatrixForTransformation(const CS123SceneTransformation &transform);

    CS123SceneGlobalData m_globalData;
    std::vector<CS123SceneLightData> m_lights;
    std::vector<CS123ScenePrimitive> m_primitives;
    std::vector<glm::mat4> m_matrices;
    // One element per primitive; null image if primitive has no texture
    std::vector<QImage> m_textures;

    // Called when the scroll wheel changes position.



};

#endif // SCENE_H
