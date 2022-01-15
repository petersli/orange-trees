#include "FruitTransformation.h"
#include <iostream>

FruitTransformation::FruitTransformation(glm::vec3 start_pos) :
    m_isFalling(false),
    m_isRolling(false),
    pos(start_pos),
    vel(glm::vec3(0.f))
{



}

FruitTransformation::~FruitTransformation(){

}

glm::mat4 FruitTransformation::updatePosition(std::unique_ptr<Terrain> &terr){

    glm::vec3 new_pos;
    glm::quat rot;
    glm::mat4 transform = glm::mat4(1.f);

    float ter_pos = terr->getHeightFromWorld(pos);
    glm::vec3 norm;

    if (m_isFalling){

        new_pos = pos + dt * vel;
        vel.y = vel.y + dt * g;
        if (new_pos.y - 0.10 <= ter_pos){ // Add radius

            if (!m_isRolling){
                norm = glm::normalize(terr->getNormalFromWorld(pos));
                glm::vec3 refl_vel = glm::reflect(vel, norm);
                vel = 0.3f * refl_vel;

            } else {
                vel.y = 0;
            }


            m_isFalling = false;
            m_isRolling = true;
        }
    }
    else if (m_isRolling){

        glm::vec3 up = glm::vec3(0., 1., 0.);
        glm::vec3 down = -up;
        norm = glm::normalize(terr->getNormalFromWorld(pos));

        glm::vec3 rotAxis = glm::normalize(glm::cross(vel, norm));

        rot = glm::angleAxis(glm::degrees(vel.length() * dt / fruit_radius), rotAxis);
        // Not working for now
        /*transform = glm::translate(transform, -pos);
        transform = glm::toMat4(rot) * transform;
        transform = glm::translate(transform, pos);*/

        new_pos = pos + dt * vel;
        if (new_pos.y - 0.05 < ter_pos){
            new_pos.y = ter_pos + 0.05;
            vel.y = 0.001;
        }
        glm::vec3 accel;
        if (new_pos.y - 0.16 > ter_pos){
            m_isFalling = true;
            //m_isRolling = false;
            accel = glm::vec3(0., 2*g, 0.);
            vel.y = glm::min(vel.y, 0.1f);
        } else {
            float theta = acos(glm::dot(up, norm));
            if (theta < .10){
                theta = 0;
            }
            float a = (5.0f / 7) * (-g * sin(theta));
            accel = a * glm::normalize(down - glm::dot(down, norm) * norm);


        }

        vel = (vel + accel*dt)*0.99f;


    } else {
        return glm::mat4(1.0f);
    }

    transform = glm::translate(transform, new_pos - pos);
    pos = new_pos;
    return transform;
}
