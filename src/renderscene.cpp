#include "renderscene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <ngl/Obj.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

RenderScene::RenderScene(){
}

void RenderScene::resizeGL(GLint width, GLint height) noexcept {
    m_width = width;
    m_height = height;
    m_ratio = m_width / float(m_height);
    m_isFBODirty = true;
}

void RenderScene::initGL() noexcept {
    ngl::NGLInit::instance();
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

void RenderScene::paintGL() noexcept {
}

void RenderScene::setViewMatrix(glm::mat4 _view){

}

void RenderScene::setProjMatrix(glm::mat4 _proj){

}
