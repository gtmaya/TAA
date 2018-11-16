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
    void setCameraLocation(glm::vec3 _location);
    void toggleAA();
    void resetAA();

  private:
    void renderCubemap();
    void renderScene(bool _cubemap, size_t _activeAAFBO);
    void blit(size_t _fbo, GLenum _texture, int _textureUnit);
    void antialias(size_t _activeAAFBO);
    void initFBO(size_t _fboID, GLenum _textureA, GLenum _textureB);
    void initEnvironment();
    void initEnvironmentSide(GLenum _target, const char *_filename);
    bool m_isFBODirty = true;
    bool m_aaDirty = true;

    std::array<std::array<GLuint, 4>, 3> m_arrFBO;

    enum m_taaFBOIndex     {taa_fboID = 0, taa_fboTextureID, taa_fboDepthID};

    size_t m_renderFBO = 0;
    size_t m_aaFBO1    = 1;
    size_t m_aaFBO2    = 2;

    GLenum m_renderFBOColour = GL_TEXTURE1;
    int    m_renderColourTU  = 1;
    GLenum m_renderFBODepth  = GL_TEXTURE2;
    int    m_renderDepthTU   = 2;
    GLenum m_aaFBOColour1    = GL_TEXTURE3;
    int    m_aaColourTU1     = 3;
    GLenum m_aaFBODepth1     = GL_TEXTURE4;
    int    m_aaDepthTU1      = 4;
    GLenum m_aaFBOColour2    = GL_TEXTURE5;
    int    m_aaColourTU2     = 5;
    GLenum m_aaFBODepth2     = GL_TEXTURE6;
    int    m_aaDepthTU2      = 6;

    GLuint m_envTex;

    GLint   m_width;
    GLint   m_height;
    GLfloat m_ratio;

    glm::vec3 m_cameraPos;

    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::mat4 m_lastView;
    glm::mat4 m_lastProj;
    glm::mat4 m_VP;
    glm::mat4 m_lastVP;
    glm::mat4 m_cube;

    std::array<ObjHandler, 6> m_arrObj;

    std::array<glm::vec2, 4> m_sampleVector {glm::vec2( 1.f,  1.f),
                                             glm::vec2( 1.f, -1.f),
                                             glm::vec2(-1.f, -1.f),
                                             glm::vec2(-1.f,  1.f)};

    float m_jitterMagnitude = 0.00055f;

    size_t m_jitterCounter = 0;

    bool m_flip = true;
    bool m_aaOn = true;

    std::array<glm::vec3, 14> m_lightPos = {glm::vec3( 1.506f, 0.815f, 0.041f),
                                            glm::vec3( 0.079f, 0.609f,-1.026f),
                                            glm::vec3( 3.534f, 0.432f, 3.913f),
                                            glm::vec3( 1.254f, 0.453f, 4.827f),
                                            glm::vec3( 0.783f, 0.837f,-0.679f),
                                            glm::vec3( 0.783f, 0.239f,-0.679f),
                                            glm::vec3( 1.568f, 0.246f, 0.037f),
                                            glm::vec3(-0.069f, 0.031f,-1.023f),
                                            glm::vec3(-3.215f, 0.069f, 1.761f),
                                            glm::vec3(-1.340f, 0.217f,-1.599f),
                                            glm::vec3( 0.073f,-0.432f, 0.045f),
                                            glm::vec3( 0.419f,-0.106f, 0.675f),
                                            glm::vec3( 0.091f, 0.822f, 1.050f),
                                            glm::vec3(-0.231f,-0.938f, 1.825f)};

    std::array<glm::vec3, 14> m_lightCol = {glm::vec3(0.551f, 0.887f, 1.000f),
                                            glm::vec3(0.432f, 0.795f, 0.995f),
                                            glm::vec3(0.485f, 0.542f, 0.562f),
                                            glm::vec3(0.314f, 0.384f, 0.296f),
                                            glm::vec3(0.390f, 0.735f, 0.999f),
                                            glm::vec3(0.817f, 0.915f, 1.000f),
                                            glm::vec3(0.443f, 0.575f, 0.739f),
                                            glm::vec3(0.187f, 0.249f, 0.317f),
                                            glm::vec3(0.152f, 0.197f, 0.237f),
                                            glm::vec3(0.270f, 0.238f, 0.158f),
                                            glm::vec3(0.084f, 0.098f, 0.045f),
                                            glm::vec3(0.223f, 0.331f, 0.407f),
                                            glm::vec3(0.209f, 0.188f, 0.150f),
                                            glm::vec3(0.441f, 0.376f, 0.276f)};
};

#endif

