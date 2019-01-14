#ifndef OBJHANDLER_H
#define OBJHANDLER_H

#include <ngl/Obj.h>
#include "shaderhandler.h"

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
    void update();
};


#endif
