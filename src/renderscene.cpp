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
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  m_arrObj[0].m_mesh = new ngl::Obj("models/blackPlastic.obj");
  m_arrObj[1].m_mesh = new ngl::Obj("models/bluePlastic.obj");
  m_arrObj[2].m_mesh = new ngl::Obj("models/gold.obj");
  m_arrObj[3].m_mesh = new ngl::Obj("models/metal.obj");
  m_arrObj[4].m_mesh = new ngl::Obj("models/translucentPlastic.obj");
  m_arrObj[5].m_mesh = new ngl::Obj("models/faceplate.obj");

  for (auto &i : m_arrObj)
  {
    i.m_mesh->createVAO();
  }

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->loadShader("EnvironmentProgram",
                     "shaders/env_vert.glsl",
                     "shaders/env_frag.glsl");


  shader->loadShader("ColourProgram",
                     "shaders/colour_v.glsl",
                     "shaders/colour_f.glsl");

  shader->loadShader("PostProcessing",
                     "shaders/post_v.glsl",
                     "shaders/post_f.glsl");

  initEnvironment();

  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",2,2,1,1,ngl::Vec3(0,1,0));
}

void RenderScene::paintGL() noexcept
{
  static bool isFirst = true;
  if (m_isFBODirty)
  {
    initFBO();
    m_isFBODirty = false;
  }

  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  GLuint pid;

  glm::mat4 M, MV, MVP;
  glm::mat3 N;

  //RENDER PREVIOUS FRAME TO SCREEN--------------------------------------------------
  if (!isFirst)
  {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,m_width,m_height);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_fboTextureID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_fboDepthID);

    shader->use("PostProcessing");
    pid = shader->getProgramID("PostProcessing");

    glUniform1i(glGetUniformLocation(pid, "fboTex"), 1);
    glUniform1i(glGetUniformLocation(pid, "fboDepthTex"), 2);
    glUniform2f(glGetUniformLocation(pid, "windowSize"), m_width, m_height);

    MVP = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f,0.0f,0.0f));
    glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"), 1, false, glm::value_ptr(MVP));

    prim->draw("plane");
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  //RENDER TO FBO--------------------------------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  //RENDER CUBEMAP-------------------------------------------------------------------
  pid = shader->getProgramID("EnvironmentProgram");
  shader->use("EnvironmentProgram");

  M = glm::mat4(1.f);
  M = glm::scale(M, glm::vec3(200.f, 200.f, 200.f));
  MV = m_cube * M;
  MVP = m_proj * MV;
  N = glm::inverse(glm::mat3(MV));

  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"),
                     1,
                     false,
                     glm::value_ptr(MVP));
  glUniformMatrix4fv(glGetUniformLocation(pid, "MV"),
                     1,
                     false,
                     glm::value_ptr(MV));
  prim->draw("cube");

  //RENDER OBJECTS-------------------------------------------------------------------
  pid = shader->getProgramID("ColourProgram");
  shader->use("ColourProgram");

  M = glm::mat4(1.f);
  MV = m_view * M;
  MVP = m_proj * MV;

  glUniformMatrix4fv(glGetUniformLocation(pid, "MV"),
                     1,
                     false,
                     glm::value_ptr(MV));


  glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"),
                     1,
                     false,
                     glm::value_ptr(MVP));
  for (auto &obj : m_arrObj)
  {
    obj.m_mesh->draw();
  }

  //THIS IS NOT THE FIRST FRAME------------------------------------------------------
  isFirst = false;
}

void RenderScene::setViewMatrix(glm::mat4 _view)
{
  m_view = _view;
}

void RenderScene::setProjMatrix(glm::mat4 _proj)
{
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
    //GLfloat anisotropy;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);

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


void RenderScene::initFBO()
{
   glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == (GL_FRAMEBUFFER_COMPLETE))
   {
     glDeleteTextures(1, &m_fboTextureID);
     glDeleteTextures(1, &m_fboDepthTextureID);
     glDeleteFramebuffers(1, &m_fboID);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   glGenTextures(1, &m_fboTextureID);
   glActiveTexture(GL_TEXTURE4);
   glBindTexture(GL_TEXTURE_2D, m_fboTextureID);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glGenTextures(1, &m_fboDepthTextureID);
   glActiveTexture(GL_TEXTURE6);
   glBindTexture(GL_TEXTURE_2D, m_fboDepthTextureID);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenFramebuffers(1, &m_fboID);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureID, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fboDepthTextureID, 0);

  GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBufs);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {std::cout<<"Help\n";}
}
