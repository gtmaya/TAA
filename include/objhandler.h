/****************************************************************************************************************
/__/\\\\\\\\\\\\\\\_____/\\\\\\\\\________/\\\\\\\\\____________/                                               |
/__\///////\\\/////____/\\\\\\\\\\\\\____/\\\\\\\\\\\\\_________/   Callum James Glover                         |
/_________\/\\\________/\\\/////////\\\__/\\\/////////\\\_______/   NCCA, Bournemouth University                |
/__________\/\\\_______\/\\\_______\/\\\_\/\\\_______\/\\\______/   s4907224@bournemouth.ac.uk                  |
/___________\/\\\_______\/\\\\\\\\\\\\\\\_\/\\\\\\\\\\\\\\\_____/   callum@glovefx.com                          |
/____________\/\\\_______\/\\\/////////\\\_\/\\\/////////\\\____/   07946 750075                                |
/_____________\/\\\_______\/\\\_______\/\\\_\/\\\_______\/\\\___/   Level 6 Computing for Animation Project     |
/______________\/\\\_______\/\\\_______\/\\\_\/\\\_______\/\\\__/   https://github.com/NCCA/CA1-2018-s4907224   |
/_______________\///________\///________\///__\///________\///__/                                               |
****************************************************************************************************************/
//---------------------------------------------------------------------------------------------------------------
/// @file objhandler.h
/// @brief Handles object properties such as their location, velocity, shader parameters and mesh
//---------------------------------------------------------------------------------------------------------------
#ifndef OBJHANDLER_H
#define OBJHANDLER_H

#include <ngl/Obj.h>
#include "shaderhandler.h"
/// @brief Handles object properties such as their location, velocity, shader parameters and mesh
struct ObjHandler
{
    ObjHandler();
    ~ObjHandler();
    ngl::Obj* m_mesh;
    ShaderProps m_shaderProps;
    glm::vec3 objectCentre;
    glm::vec3 angularVelocity;
    glm::vec3 linearVelocity;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::mat4 previousMVP;
    std::chrono::high_resolution_clock::time_point m_now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point m_last = std::chrono::high_resolution_clock::now();
    void update();
};


#endif
