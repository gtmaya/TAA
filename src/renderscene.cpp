#include "renderscene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <ngl/Obj.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

RenderScene::RenderScene() : m_width(1),
                             m_height(1),
                             m_ratio(1.0f)
{}

RenderScene::~RenderScene() = default;

void RenderScene::resizeGL(GLint _width, GLint _height) noexcept
{
  m_width = _width;
  m_height = _height;
  m_ratio = m_width / float(m_height);
  m_isFBODirty = true;
}

void RenderScene::initGL() noexcept
{
  ngl::NGLInit::instance();
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  m_arrObj[0].m_mesh = new ngl::Obj("models/scene.obj");

  for (auto &i : m_arrObj)
  {
    i.m_mesh->createVAO();
  }

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->loadShader("EnvironmentProgram",
                     "shaders/env_v.glsl",
                     "shaders/env_f.glsl");


  shader->loadShader("ColourProgram",
                     "shaders/colour_v.glsl",
                     "shaders/colour_f.glsl");

  shader->loadShader("PostProcessing",
                     "shaders/post_v.glsl",
                     "shaders/post_f.glsl");

  shader->loadShader("blitShader",
                     "shaders/blit_v.glsl",
                     "shaders/blit_f.glsl");

  initEnvironment();

  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",2,2,1,1,ngl::Vec3(0,1,0));
}

void RenderScene::paintGL() noexcept
{
  if (m_isFBODirty)
  {
    initFBO(taa_fboA, m_renderFBOColour, m_renderFBODepth);
    initFBO(taa_fboB, m_aaFBOColour, m_aaFBODepth);
    m_isFBODirty = false;
  }

  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  GLuint pid;


  //RENDER TO FBO--------------------------------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[m_renderFBO][taa_fboID]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  //RENDER CUBEMAP-------------------------------------------------------------------
  //renderCubemap();

  //RENDER OBJECTS-------------------------------------------------------------------
  static size_t count;
  m_lastVP = m_VP;
  m_VP = m_proj * m_view;
  m_VP = glm::translate(m_VP, m_sampleVector[count]);
  renderScene();

  //PERFOM ANTI ALIASING-------------------------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[m_aaFBO][taa_fboID]);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);
  glActiveTexture(m_renderFBOColour);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboTextureID]);
  glActiveTexture(m_renderFBODepth);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboDepthID]);
  glActiveTexture(m_aaFBOColour);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_aaFBO][taa_fboTextureID]);
  glActiveTexture(m_aaFBODepth);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_aaFBO][taa_fboDepthID]);

  shader->use("PostProcessing");
  pid = shader->getProgramID("PostProcessing");

  glm::mat4 invP, invV, invVP, invVPPREVIOUS, vpPREVIOUS;
  invP = glm::inverse(m_proj);
  invV = glm::inverse(m_view);
  invVP = glm::inverse(m_VP);
  vpPREVIOUS = m_lastProj * m_lastView;
  invVPPREVIOUS = glm::inverse(vpPREVIOUS);

  glUniformMatrix4fv(glGetUniformLocation(pid, "inverseViewProjectionCURRENT"),
                     1,
                     false,
                     glm::value_ptr(invVP));
  glUniformMatrix4fv(glGetUniformLocation(pid, "viewProjectionPREVIOUS"),
                     1,
                     false,
                     glm::value_ptr(m_lastVP));
  glUniform3fv(glGetUniformLocation(pid, "jitter"),
               1,
               glm::value_ptr(m_sampleVector[count]));

  glUniform1i(glGetUniformLocation(pid, "fboTex"), m_renderColourTU);
  glUniform1i(glGetUniformLocation(pid, "fboDepthTex"), m_renderDepthTU);
  glUniform1i(glGetUniformLocation(pid, "pastfboTex"), m_aaColourTU);
  glUniform1i(glGetUniformLocation(pid, "pastfboDepthTex"), m_aaDepthTU);
  glUniform2f(glGetUniformLocation(pid, "windowSize"), m_width, m_height);

  glm::mat4 screenMVP = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f,0.0f,0.0f));
  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"), 1, false, glm::value_ptr(screenMVP));

  prim->draw("plane");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);
  glActiveTexture(m_aaFBOColour);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_aaFBO][taa_fboTextureID]);

  pid = shader->getProgramID("blitShader");
  shader->use("blitShader");

  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"), 1, false, glm::value_ptr(screenMVP));
  glUniform1i(glGetUniformLocation(pid, "inputTex"), m_aaColourTU);
  glUniform2f(glGetUniformLocation(pid, "windowSize"), m_width, m_height);

  prim->draw("plane");

  glBindTexture(GL_TEXTURE_2D, 0);

  count++;
  if (count > 3) {count = 0;}
}

void RenderScene::renderCubemap()
{
  glm::mat4 cubeM, cubeMV, cubeMVP;
  glm::mat3 cubeN;

  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  GLuint pid = shader->getProgramID("EnvironmentProgram");
  shader->use("EnvironmentProgram");



  cubeM = glm::mat4(1.f);
  cubeM = glm::scale(cubeM, glm::vec3(200.f, 200.f, 200.f));
  cubeMV = m_cube * cubeM;
  cubeMVP = m_proj * cubeMV;
  cubeN = glm::inverse(glm::mat3(cubeMV));

  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"),
                     1,
                     false,
                     glm::value_ptr(cubeMVP));
  glUniformMatrix4fv(glGetUniformLocation(pid, "MV"),
                     1,
                     false,
                     glm::value_ptr(cubeMV));

  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  prim->draw("cube");
}

void RenderScene::renderScene()
{
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  GLuint pid = shader->getProgramID("ColourProgram");
  shader->use("ColourProgram");

  glm::mat4 M, MV, MVP;
  glm::mat3 N;
  M = glm::mat4(1.f);
  MV = m_view * M;
  MVP = m_VP * M;
  N = glm::inverse(glm::mat3(MV));

  glUniformMatrix4fv(glGetUniformLocation(pid, "MV"),
                     1,
                     false,
                     glm::value_ptr(MV));
  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"),
                     1,
                     false,
                     glm::value_ptr(MVP));
  glUniformMatrix3fv(glGetUniformLocation(pid, "N"),
                     1,
                     true,
                     glm::value_ptr(N));

  for (auto &obj : m_arrObj)
  {
    obj.m_mesh->draw();
  }
}

void RenderScene::setViewMatrix(glm::mat4 _view)
{
  m_lastView = m_view;
  m_view = _view;
}

void RenderScene::setProjMatrix(glm::mat4 _proj)
{
  m_lastProj = m_proj;
  m_proj = _proj;
}

void RenderScene::setCubeMatrix(glm::mat4 _cube)
{
  m_cube = _cube;
}

void RenderScene::initEnvironment()
{
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &m_envTex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_envTex);

  initEnvironmentSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "images/nz.png");
  initEnvironmentSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "images/pz.png");
  initEnvironmentSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "images/ny.png");
  initEnvironmentSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "images/py.png");
  initEnvironmentSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "images/nx.png");
  initEnvironmentSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "images/px.png");

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_AUTO_GENERATE_MIPMAP, GL_TRUE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  GLfloat anisotropy;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use("EnvironmentProgram");
  shader->setUniform("envMap", 0);
}

void RenderScene::initEnvironmentSide(GLenum target, const char *filename)
{
    ngl::Image img(filename);
    glTexImage2D (
      target,           // The target (in this case, which side of the cube)
      0,                // Level of mipmap to load
      img.format(),     // Internal format (number of colour components)
      img.width(),      // Width in pixels
      img.height(),     // Height in pixels
      0,                // Border
      img.format(),     // Format of the pixel data
      GL_UNSIGNED_BYTE, // Data type of pixel data
      img.getPixels()   // Pointer to image data in memory
    );
}


void RenderScene::initFBO(size_t _fboID, GLenum _textureA, GLenum _textureB)
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[_fboID][taa_fboID]);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == (GL_FRAMEBUFFER_COMPLETE))
  {
   glDeleteTextures(1, &m_arrFBO[_fboID][taa_fboTextureID]);
   glDeleteTextures(1, &m_arrFBO[_fboID][taa_fboDepthID]);
   glDeleteFramebuffers(1, &m_arrFBO[_fboID][taa_fboID]);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenTextures(1, &m_arrFBO[_fboID][taa_fboTextureID]);
  glActiveTexture(_textureA);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboTextureID]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &m_arrFBO[_fboID][taa_fboDepthID]);
  glActiveTexture(_textureB);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboDepthID]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenFramebuffers(1, &m_arrFBO[_fboID][taa_fboID]);
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[_fboID][taa_fboID]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboTextureID], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboDepthID], 0);

  GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBufs);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {std::cout<<"Help\n";}
}
