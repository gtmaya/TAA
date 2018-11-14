#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <ngl/Obj.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "objhandler.h"
#include "shaderhandler.h"

class RenderScene
{
  public:
    RenderScene();
    ~RenderScene();
    void paintGL() noexcept;
    void initGL() noexcept;
    void resizeGL(GLint /*width*/, GLint /*height*/) noexcept;
    void setViewMatrix(glm::mat4 _view);
    void setProjMatrix(glm::mat4 _proj);
    void setCubeMatrix(glm::mat4 _cube);

  private:
    void renderCubemap();
    void renderScene(bool _cubemap);
    void blit(size_t _fbo, GLenum _texture, int _textureUnit);
    void antialias();
    void initFBO(size_t _fboID, GLenum _textureA, GLenum _textureB);
    void initEnvironment();
    void initEnvironmentSide(GLenum _target, const char *_filename);
    bool m_isFBODirty = true;
    bool m_firstFrame = true;

    std::array<std::array<GLuint, 4>, 2> m_arrFBO;

    enum m_taaFBOIndex     {taa_fboID = 0, taa_fboTextureID, taa_fboDepthID};

    size_t m_renderFBO = 0;
    size_t m_aaFBO     = 1;

    GLenum m_renderFBOColour = GL_TEXTURE1;
    int    m_renderColourTU  = 1;
    GLenum m_renderFBODepth  = GL_TEXTURE2;
    int    m_renderDepthTU   = 2;
    GLenum m_aaFBOColour     = GL_TEXTURE3;
    int    m_aaColourTU      = 3;
    GLenum m_aaFBODepth      = GL_TEXTURE4;
    int    m_aaDepthTU       = 4;

    GLuint m_envTex;

    GLint   m_width;
    GLint   m_height;
    GLfloat m_ratio;

    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::mat4 m_lastView;
    glm::mat4 m_lastProj;
    glm::mat4 m_VP;
    glm::mat4 m_lastVP;
    glm::mat4 m_cube;

    std::array<ObjHandler, 1> m_arrObj;

    std::array<glm::vec3, 4> m_sampleVector {glm::vec3(0.f,  0.00001f,  0.00001f),
                                             glm::vec3(0.f,  0.00001f, -0.00001f),
                                             glm::vec3(0.f, -0.00001f, -0.00001f),
                                             glm::vec3(0.f, -0.00001f,  0.00001f)};

    size_t m_jitterCounter = 0;


};

#endif

