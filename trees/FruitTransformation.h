#ifndef FRUITTRANSFORMATION_H
#define FRUITTRANSFORMATION_H

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "trees/terrain.h"


const float dt = 1.0f/30;
const float mu = 0.5;
const float g = -.98;
const float fruit_radius = .15;

class FruitTransformation
{
public:
    FruitTransformation(glm::vec3 start_pos);
    ~FruitTransformation();

    bool m_isFalling;
    bool m_isRolling;

    glm::vec3 pos;
    glm::vec3 vel;


    glm::mat4 updatePosition(std::unique_ptr<Terrain> &terr);
    void setPos(glm::vec3 start_pos);
};

#endif // FRUITTRANSFORMATION_H
