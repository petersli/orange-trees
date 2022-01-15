#include "RayScene.h"
#include "Settings.h"
#include "RayGeometry.h"
#include "CS123SceneData.h"
#include "qcoreapplication.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>


RayScene::RayScene(Scene &scene) :
    Scene(scene),
    m_viewPlaneDepth(1.0f),
    m_rendering(false),
    m_recursionDepth(0)
{
    m_implicitShape = std::make_unique<ImplicitShape>();
    m_implicitSphere = std::make_unique<ImplicitSphere>();
    m_implicitCylinder = std::make_unique<ImplicitCylinder>();
    m_implicitCone = std::make_unique<ImplicitCone>();
    m_implicitCube = std::make_unique<ImplicitCube>();
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.
}

RayScene::~RayScene()
{
}

/**
 *  Main ray tracing loop. For each pixel, find the closest intersecting object and
 *  compute the color at the intersection point. If there is no intersecting object,
 *  set the pixel to black.
 */
void RayScene::renderRayScene(Canvas2D *canvas, CS123SceneCameraData *camera, int width, int height) {
    m_rendering = true;
    canvas->resize(width, height);

    // Convert degrees to radians
    float aspectRatio = static_cast<float>(canvas->width()) / canvas->height();
    float heightAngle = camera->heightAngle * PI / 180.0f;
    float widthAngle = heightAngle * aspectRatio;

    // Set up camera transformations
    glm::vec3 cameraPos = glm::vec3(camera->pos);
    glm::mat4 worldToCameraSpace = getCameraMatrix(camera);
    glm::mat4 cameraToWorldSpace = glm::inverse(worldToCameraSpace);
    glm::mat3 cameraToWorldSpace3 = glm::mat3(cameraToWorldSpace);

    RGBA* pixels = canvas->data();
    for (int col = 0; col < width; col++) {
        for (int row = 0; row < height; row++) {
            // Check if rendering has been cancelled
            if (!m_rendering) return;
            // Reset recursion depth for new pixel
            m_recursionDepth = 0;
            // Generate ray through view plane
            glm::vec3 cameraSpaceDirection = getRayCameraDirection(
                        widthAngle, heightAngle, width, height, col, row);
            // Transform to world space and trace ray
            glm::vec3 worldSpaceDirection = cameraToWorldSpace3 * cameraSpaceDirection;
            glm::vec4 color = traceRay(Ray(cameraPos, worldSpaceDirection));
            int redInt = static_cast<int>(color[0] * 255.0f);
            int greenInt = static_cast<int>(color[1] * 255.0f);
            int blueInt = static_cast<int>(color[2] * 255.0f);
            canvas->setPixelColor(pixels, row, col, RGBA(redInt, greenInt, blueInt, 255));
        }
        // Update canvas
        canvas->update();
        QCoreApplication::processEvents();
    }
}

/**
 *  Trace a ray, returning the color at the intersection point, or black
 *  if there is no intersection. Recursively computes inter-object reflection by
 *  tracing reflected rays until max recursion depth is reached or no intersection
 *  is found.
 */
glm::vec4 RayScene::traceRay(Ray ray) {
    // Find and store the nearest intersection with a primitive object
    IntersectionWithPrimitive nearestIntersection = rayObjectIntersection(ray);
    // If no valid intersection, return black
    if (nearestIntersection.t < EPSILON) {
        return glm::vec4(0.f);
    }
    // Otherwise, recursively compute color at intersection point
    else {
        int primitiveIndex = nearestIntersection.primitiveIndex;
        glm::vec3 objectSpacePos = nearestIntersection.objectSpacePos;
        // Transform object-space normal to world-space
        glm::vec3 objectSpaceNormal = nearestIntersection.objectSpaceNormal;
        glm::mat4 ctm = m_matrices[primitiveIndex];
        glm::mat3 inverseCtm3 = glm::inverse(glm::mat3(ctm));
        glm::mat3 normalToWorldSpace = glm::transpose(inverseCtm3);
        glm::vec3 worldSpaceNormal = glm::normalize(normalToWorldSpace * objectSpaceNormal);
        // Transform intersection point to world-space
        glm::vec3 worldSpacePos = glm::vec3(ctm * glm::vec4(objectSpacePos, 1.0f));
        // Get texture color from texture map and UV
        CS123SceneMaterial material = m_primitives[primitiveIndex].material;
        glm::vec4 textureColor;
        QImage texture = m_textures[primitiveIndex];
        if (!texture.isNull()) {
            textureColor = getTextureColor(texture, material.textureMap,
                                           nearestIntersection.uv);
        } else {
            textureColor = glm::vec4(0.f);
        }
        // Compute recursive reflections
        glm::vec4 reflectedColor = glm::vec4(0);
        glm::vec3 eye = ray.startPoint;
        if (settings.useReflection) {
            Ray reflectedRay = Ray(worldSpacePos,
                                   glm::reflect(ray.direction, worldSpaceNormal));
            // Check whether this point will contribute non-negligible reflectance
            glm::vec4 maxPossibleReflection = lightingEquation(worldSpacePos,
                                                               worldSpaceNormal,
                                                               material,
                                                               eye, textureColor,
                                                               glm::vec4(1, 1, 1, 1));
            bool significantReflectance =
                    glm::length(glm::vec3(maxPossibleReflection)) > minReflectedContribution;
            if (significantReflectance && m_recursionDepth < maxRecursionDepth) {
                m_recursionDepth++;
                reflectedColor = traceRay(reflectedRay);
            }
        }
        // Finally, compute color from lighting equation
        return lightingEquation(worldSpacePos, worldSpaceNormal, material,
                                     eye, textureColor, reflectedColor);
    }
}

/**
 *  For each primitive, check if the ray intersects it. Return an Intersection containing
 *  the t-value of the closest intersected object.
 */
IntersectionWithPrimitive RayScene::rayObjectIntersection(Ray ray) {
    std::vector<IntersectionWithPrimitive> intersections;
    for (int i = 0; i < m_primitives.size(); i++) {
        // Cumulative transformation matrix
        glm::mat4 ctm = m_matrices[i];
        glm::mat4 inverseCtm = glm::inverse(ctm);
        // Convert to object space using inverse of the CTM
        glm::vec3 objectSpaceDirection = glm::mat3(inverseCtm) * ray.direction;
        glm::vec3 objectSpaceEye = glm::vec3(inverseCtm * glm::vec4(ray.startPoint, 1.0f));
        Ray objectSpaceRay = Ray(objectSpaceEye, objectSpaceDirection);
        // Check for intersection
        CS123ScenePrimitive primitive = m_primitives[i];
        if (primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
            Intersection intersection = m_implicitCylinder->intersect(objectSpaceRay);
            UV uv = m_implicitCylinder->mapToUV(intersection.objectSpacePos);
            intersections.push_back(IntersectionWithPrimitive(intersection, i, uv));
        } else if (primitive.type == PrimitiveType::PRIMITIVE_CONE) {
            Intersection intersection = m_implicitCone->intersect(objectSpaceRay);
            UV uv = m_implicitCone->mapToUV(intersection.objectSpacePos);
            intersections.push_back(IntersectionWithPrimitive(intersection, i, uv));
        } else if (primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
            Intersection intersection = m_implicitCube->intersect(objectSpaceRay);
            UV uv = m_implicitCube->mapToUV(intersection.objectSpacePos);
            intersections.push_back(IntersectionWithPrimitive(intersection, i, uv));
        } else if (primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
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
 *  Compute color at point of intersection based on Phong illumination model.
 *  Uses scene lights, global data, object material, and normal vector at intersection.
 */
glm::vec4 RayScene::lightingEquation(glm::vec3 intersectionPoint,
                                     glm::vec3 normal,
                                     CS123SceneMaterial objectMaterial,
                                     glm::vec3 eye,
                                     glm::vec4 textureColor,
                                     glm::vec4 reflectedColor) {
    float red = lightingEquationForChannel(intersectionPoint, normal, objectMaterial,
                                 eye, textureColor, reflectedColor, 0);
    float green = lightingEquationForChannel(intersectionPoint, normal, objectMaterial,
                                 eye, textureColor, reflectedColor, 1);
    float blue = lightingEquationForChannel(intersectionPoint, normal, objectMaterial,
                                 eye, textureColor, reflectedColor, 2);
    return glm::vec4(red, green, blue, 1.f);
}

/**
 *  Compute intensity in [0,1] for a given RGB channel at point of intersection.
 *  Uses scene lights, global data, object material, and normal vector at intersection.
 *  Based on Phong illumination model.
 */
float RayScene::lightingEquationForChannel(
        glm::vec3 intersectionPoint,
        glm::vec3 normal,
        CS123SceneMaterial objectMaterial,
        glm::vec3 eye,
        glm::vec4 textureColor,
        glm::vec4 reflectedColor,
        int channelIndex)
{
    // Sum diffuse + specular contribution for all lights
    float totalLightIntensity = 0;
    for (int i = 0; i < m_lights.size(); i++) {
        CS123SceneLightData light = m_lights[i];
        float contribution = getLightContribution(light, intersectionPoint, normal,
                                                  objectMaterial, eye, textureColor, channelIndex);
        totalLightIntensity = totalLightIntensity + contribution;
    }

    // Ambient intensity in [0, 1] for this channel
    float objectAmbient = objectMaterial.cAmbient[channelIndex];
    // Global ambient coefficient
    float ka = m_globalData.ka;
    // Object reflective intensity in [0, 1] for this channel
    float objectReflective = objectMaterial.cReflective[channelIndex];
    // Global specular coefficient
    float ks = m_globalData.ks;
    // Get reflective contribution
    float reflectiveContribution = ks * objectReflective * reflectedColor[channelIndex];
    // Compute overall lighting
    float computedLighting = ka * objectAmbient
            + totalLightIntensity + reflectiveContribution;
    return std::min(computedLighting, 1.f);
}

/**
 *  Gets the contribution (diffuse + specular) of a single light in a scene.
 *  Adds attenuation for point lights.
 */
float RayScene::getLightContribution(
        CS123SceneLightData light,
        glm::vec3 intersectionPoint,
        glm::vec3 normal,
        CS123SceneMaterial objectMaterial,
        glm::vec3 eye,
        glm::vec4 textureColor,
        int channelIndex)
{
    // Global diffuse coefficient
    float kd = m_globalData.kd;
    // Global specular coefficient
    float ks = m_globalData.ks;
    // Diffuse intensity in [0, 1] for this channel
    float objectDiffuse = objectMaterial.cDiffuse[channelIndex];
    // Specular intensity in [0, 1] for this channel
    float objectSpecular = objectMaterial.cSpecular[channelIndex];

    glm::vec3 surfaceToLight;
    float attenuation = 1.0f;
    if (light.type == LightType::LIGHT_POINT && settings.usePointLights) {
        surfaceToLight = glm::normalize(glm::vec3(light.pos) - intersectionPoint);
        // Compute attenuation for point lights
        glm::vec3 attenuationFunc = light.function;
        float constantAtt = attenuationFunc[0];
        float linearAtt = attenuationFunc[1];
        float quadraticAtt = attenuationFunc[2];
        float distToLight = glm::distance(intersectionPoint, glm::vec3(light.pos));
        float rawAttenuation = 1.0f /
                (constantAtt + linearAtt * distToLight + quadraticAtt * distToLight * distToLight);
        attenuation = std::min(rawAttenuation, 1.0f);
    } else if (light.type == LightType::LIGHT_DIRECTIONAL && settings.useDirectionalLights) {
        surfaceToLight = glm::normalize(-1.0f * glm::vec3(light.dir));
    } else {
        // No contribution from light types we don't handle
        return 0.f;
    }

    // Check if surface point is in shadow, return 0 contribution if so
    Ray shadowRay = Ray(intersectionPoint, surfaceToLight);
    if (isInShadow(shadowRay, light)) {
        return 0.f;
    }

    // Dot product for diffuse lighting calculation
    float normalDotLight = glm::dot(normal, surfaceToLight);
    // Dot product for specular lighting calculation
    glm::vec3 reflectedPointToLight =
            glm::normalize(2.0f * normal * glm::dot(normal, surfaceToLight) - surfaceToLight);
    glm::vec3 lineOfSight = glm::normalize(eye - intersectionPoint);
    float reflectionDotLineOfSight = glm::dot(reflectedPointToLight, lineOfSight);

    // Add diffuse and specular contributions
    float contribution = 0;
    // If dot product is negative, the angle between the light and the normal is
    // greater than 90. These checks prevent adding negative light contributions.
    if (normalDotLight > EPSILON) {
        float diffuseColor = kd * objectDiffuse;
        // Blend diffuse color with texture color if enabled
        float blend;
        if (settings.useTextureMapping) {
            blend = objectMaterial.blend;
        } else {
            blend = 0.f;
        }
        float textureIntensity = textureColor[channelIndex];
        float diffuseTextureBlend = blend * textureIntensity + (1.f - blend) * diffuseColor;
        contribution = contribution + diffuseTextureBlend * normalDotLight;
    }
    if (reflectionDotLineOfSight > EPSILON) {
        float shininess = objectMaterial.shininess;
        float specularContribution = ks * objectSpecular *
                std::pow(reflectionDotLineOfSight, shininess);
        contribution = contribution + specularContribution;
    }

    float lightIntensity = light.color[channelIndex];
    return attenuation * lightIntensity * contribution;
}

/** Convert unit-square (u, v) coords into texture image coords and return resulting color */
glm::vec4 RayScene::getTextureColor(const QImage &texture, const CS123SceneFileMap &map, const UV &uv) {
    float u = uv.u;
    float v = uv.v;
    float repeatU = map.repeatU;
    float repeatV = map.repeatV;
    int width = texture.width();
    int height = texture.height();
    int horizCoord = static_cast<int>(u * repeatU * width) % width;
    int vertCoord = static_cast<int>(v * repeatV * height) % height;
    QRgb color = texture.pixel(horizCoord, vertCoord);
    float red = qRed(color) / 255.0f;
    float green = qGreen(color) / 255.0f;
    float blue = qBlue(color) / 255.0f;
    return glm::vec4(red, green, blue, 1.0f);
}

/**
 *  Get direction of ray through pixel in camera coordinate space,
 *  i.e. when (u, v, w) camera space is coincident with (x, y, z) world space
 */
glm::vec3 RayScene::getRayCameraDirection(float widthAngle, float heightAngle,
                                          int width, int height, int col, int row) {
    float x = (col + 0.5f) / width - 0.5f;
    float y = 0.5f - (row + 0.5f) / height;
    float U = 2 * m_viewPlaneDepth * tan(widthAngle / 2.0f);
    float V = 2 * m_viewPlaneDepth * tan(heightAngle / 2.0f);
    glm::vec3 viewPlanePoint = glm::vec3(U * x, V * y, -m_viewPlaneDepth);
    return glm::normalize(viewPlanePoint);
}

/** Compute matrix that maps world space to the canonical camera space */
glm::mat4 RayScene::getCameraMatrix(CS123SceneCameraData *camera) {
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

/** Return whether the surface point given by the shadow ray is in shadow */
bool RayScene::isInShadow(Ray shadowRay, CS123SceneLightData light) {
    if (!settings.useShadows) {
        return false;
    }
    float shadowRayTVal = rayObjectIntersection(shadowRay).t;
    if (light.type == LightType::LIGHT_POINT && settings.usePointLights) {
        float distanceToLight = glm::distance(shadowRay.startPoint, glm::vec3(light.pos));
        // For point lights, if shadow ray intersects an object on the way to the light,
        // the surface is occluded
        if (shadowRayTVal > EPSILON && shadowRayTVal < distanceToLight) {
            return true;
        }
    } else if (light.type == LightType::LIGHT_DIRECTIONAL && settings.useDirectionalLights) {
        // For directional lights, if shadow ray intersects any object, the surface is occluded
        if (shadowRayTVal > EPSILON) {
            return true;
        }
    }
    return false;
}

/** Stop the currently executing render */
void RayScene::stopRendering() {
    m_rendering = false;
}
