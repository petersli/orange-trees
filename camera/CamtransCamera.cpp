/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>

CamtransCamera::CamtransCamera()
{
   m_near = 1.0f;
   m_far = 30.0f;
   m_thetaH = 60.0f;
   m_aspectRatio = 1.0f;
   orientLook(glm::vec4(3.5f, 2.f, 3.5f, 1.f),
              glm::vec4(-1.f, 0.2f, -1.f, 0.f),
              glm::vec4(0.f, 1.f, 0.f, 0.f));
}

void CamtransCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    updateProjectionMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    glm::mat4 projection = m_perspectiveTransformation * m_scaleMatrix;
    projection[2][2] = -projection[2][2];
    projection[3][2] = -projection[3][2];
    return projection;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    return m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const {
    return m_look;
}

glm::vec4 CamtransCamera::getUp() const {
    return m_up;
}

float CamtransCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    return m_thetaH;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    m_eye = eye;
    m_look = look;
    m_up = up;

    m_w = glm::normalize(-1.0f * m_look);
    m_v = glm::normalize(m_up - glm::dot(m_up, m_w) * m_w);
    m_u = glm::vec4(glm::cross(glm::vec3(m_v), glm::vec3(m_w)), 0);

    updateViewMatrix();
    updateProjectionMatrix();
}

void CamtransCamera::setHeightAngle(float h) {
    m_thetaH = h;
    updateProjectionMatrix();
}

void CamtransCamera::translate(const glm::vec4 &v) {
   m_eye = m_eye + v;
   updateViewMatrix();
}

glm::vec4 CamtransCamera::getU() const {
    return m_u;
}

glm::vec4 CamtransCamera::getV() const {
    return m_v;
}

glm::vec4 CamtransCamera::getW() const {
    return m_w;
}

void CamtransCamera::rotateU(float degrees) {
    float theta = glm::radians(degrees);
    // Rotation about axis
    glm::mat3 rotateAboutX = glm::mat3(1, 0, 0,
                                       0, cos(theta), sin(theta),
                                       0, -sin(theta), cos(theta));
    glm::vec3 newY = rotateAboutX * glm::vec3(0, 1, 0);
    glm::vec3 newZ = rotateAboutX * glm::vec3(0, 0, 1);
    glm::mat4 inverseView = glm::inverse(getViewMatrix());
    m_v = inverseView * glm::vec4(newY, 0);
    m_w = inverseView * glm::vec4(newZ, 0);
    updateViewMatrix();
}

void CamtransCamera::rotateV(float degrees) {
    float theta = glm::radians(degrees);
    // Rotation about axis
    glm::mat3 rotateAboutY = glm::mat3(cos(theta), 0, -sin(theta),
                                  0, 1, 0,
                                  sin(theta), 0, cos(theta));
    glm::vec3 newX = rotateAboutY * glm::vec3(1, 0, 0);
    glm::vec3 newZ = rotateAboutY * glm::vec3(0, 0, 1);
    glm::mat4 inverseView = glm::inverse(getViewMatrix());
    m_u = inverseView * glm::vec4(newX, 0);
    m_w = inverseView * glm::vec4(newZ, 0);
    updateViewMatrix();
}

void CamtransCamera::rotateW(float degrees) {
    float theta = glm::radians(degrees);
    // Rotation about axis
    glm::mat3 rotateAboutZ = glm::mat3(cos(theta), sin(theta), 0,
                                  -sin(theta), cos(theta), 0,
                                  0, 0, 1);
    glm::vec3 newX = rotateAboutZ * glm::vec3(1, 0, 0);
    glm::vec3 newY = rotateAboutZ * glm::vec3(0, 1, 0);
    glm::mat4 inverseView = glm::inverse(getViewMatrix());
    m_u = inverseView * glm::vec4(newX, 0);
    m_v = inverseView * glm::vec4(newY, 0);
    updateViewMatrix();
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}

void CamtransCamera::updatePerspectiveMatrix() {
    float c = -m_near / m_far;
    // Column-major definition
    m_perspectiveTransformation = glm::mat4(1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, 1.0f/(1.0f + c), -1,
                                            0, 0, -c/(1.0f + c), 0);
}

void CamtransCamera::updateProjectionMatrix() {
    updateScaleMatrix();
    updatePerspectiveMatrix();
}

void CamtransCamera::updateRotationMatrix() {
    m_rotationMatrix = glm::transpose(glm::mat4(m_u, m_v, m_w, glm::vec4(0.f, 0.f, 0.f, 1.0f)));
}

void CamtransCamera::updateScaleMatrix() {
    // Aspect ratio relates tangents of theta_W and theta_H
    float tanHalfThetaH = tan(glm::radians(m_thetaH / 2.0f));
    float tanHalfThetaW = tanHalfThetaH * m_aspectRatio;
    float topLeft = 1.0f / (m_far * tanHalfThetaW);
    float middle = 1.0f / (m_far *tanHalfThetaH);
    float bottomRight = 1.0f / m_far;
    // Diagonal matrix
    m_scaleMatrix = glm::mat4(topLeft, 0, 0, 0,
                              0, middle, 0, 0,
                              0, 0, bottomRight, 0,
                              0, 0, 0, 1);
}

void CamtransCamera::updateTranslationMatrix() {
    // Column-major definition
    m_translationMatrix = glm::mat4(1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0,
                                    -m_eye.x, -m_eye.y, -m_eye.z, 1);
}

void CamtransCamera::updateViewMatrix() {
    updateRotationMatrix();
    updateTranslationMatrix();
    glm::mat4 inverseView = glm::inverse(getViewMatrix());
    // Transform canonical camera space values
    m_up = inverseView * glm::vec4(0, 1, 0, 0);
    m_look = inverseView * glm::vec4(0, 0, -1, 0);
}



