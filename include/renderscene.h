#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <ngl/Obj.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RenderScene
{
  public:
    RenderScene();
    void paintGL() noexcept;
    void initGL() noexcept;
    void resizeGL(GLint /*width*/, GLint /*height*/) noexcept;
    void setViewMatrix(glm::mat4 _view);
    void setProjMatrix(glm::mat4 _proj);

  private:
    void initFBO();
    bool m_isFBODirty = true;
    GLuint m_fboId;
    GLuint m_fboTextureId;
    GLuint m_fboDepthId;
    GLint m_width;
    GLint m_height;
    GLfloat m_ratio;
    glm::mat4 m_view;
    glm::mat4 m_proj;
    std::array<glm::vec3, 1000> m_randPos;
};

#endif
