#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"
#include "Canvas2D.h"
#include "RayGeometry.h"
#include "ImplicitShape.h"
#include "ImplicitSphere.h"
#include "ImplicitCylinder.h"
#include "ImplicitCone.h"
#include "ImplicitCube.h"


#include <vector>


/** The maximum amount of times to recur when computing object reflection */
const int maxRecursionDepth = 10;

/**
 *  Given a glm::vec3 color vector for the maximum possible reflective
 *  contribution from a point, the norm of the vector must be greater than
 *  this value is order to continue recursion.
 */
const float minReflectedContribution = 0.01;


/**
 * @class RayScene
 *
 * A scene to be rendered via ray tracing.
 */
class RayScene : public Scene {
public:
    RayScene(Scene &scene);
    virtual ~RayScene();
    void renderRayScene(Canvas2D *canvas, CS123SceneCameraData *camera, int width, int height);
    void stopRendering();
private:
    // Implicit shape definitions
    std::unique_ptr<ImplicitShape> m_implicitShape;
    std::unique_ptr<ImplicitSphere> m_implicitSphere;
    std::unique_ptr<ImplicitCylinder> m_implicitCylinder;
    std::unique_ptr<ImplicitCone> m_implicitCone;
    std::unique_ptr<ImplicitCube> m_implicitCube;

    // Primary ray generation
    float m_viewPlaneDepth;
    glm::vec3 getRayCameraDirection(float widthAngle, float heightAngle,
                                    int width, int height, int col, int row);

    // Ray-object intersection
    IntersectionWithPrimitive rayObjectIntersection(Ray ray);
    glm::mat4 getCameraMatrix(CS123SceneCameraData *camera);

    // Lighting computation and texture mapping
    glm::vec4 traceRay(Ray ray);
    glm::vec4 lightingEquation(glm::vec3 intersectionPoint,
                           glm::vec3 normal,
                           CS123SceneMaterial objectMaterial,
                           glm::vec3 eye,
                           glm::vec4 textureColor,
                           glm::vec4 reflectedColor);
    float lightingEquationForChannel(glm::vec3 intersectionPoint,
                           glm::vec3 normal,
                           CS123SceneMaterial objectMaterial,
                           glm::vec3 eye,
                           glm::vec4 textureColor,
                           glm::vec4 reflectedColor,
                           int channelIndex);
    float getLightContribution(CS123SceneLightData light,
                               glm::vec3 intersectionPoint,
                               glm::vec3 normal,
                               CS123SceneMaterial objectMaterial,
                               glm::vec3 eye,
                               glm::vec4 textureColor,
                               int channelIndex);
    glm::vec4 getTextureColor(const QImage &texture,
                              const CS123SceneFileMap &map, const UV &uv);
    bool isInShadow(Ray shadowRay, CS123SceneLightData light);

    // State variables
    bool m_rendering;
    int m_recursionDepth;
};

#endif // RAYSCENE_H
