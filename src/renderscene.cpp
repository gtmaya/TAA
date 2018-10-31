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
{
//  std::srand(std::time(nullptr));
//  for (int i = 0; i < 1000; i++)
//  {
//    m_randPos[i] = glm::vec3(std::rand()/float(RAND_MAX) * 200.f, std::rand()/float(RAND_MAX) * 200.f, std::rand()/float(RAND_MAX) * 200.f);
//    for (int j = 0; j < 3; j++)
//    {
//      if (std::rand()/float(RAND_MAX) > 0.5f)
//      {
//        m_randPos[i].operator[](j) *= -1;
//      }
//    }
//  }
}

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

  std::cout<<"Hello?\n";
  for (auto &i : m_arrObj)
  {
    static int e = 0;
    std::cout<<e<<'\n';
    i.m_mesh->createVAO();
    e++;
  }

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->loadShader("EnvironmentProgram",
                     "shaders/env_vert.glsl",
                     "shaders/env_frag.glsl");


  shader->loadShader("ColourProgram",
                     "shaders/colour_v.glsl",
                     "shaders/colour_f.glsl");

  initEnvironment();
}

void RenderScene::paintGL() noexcept
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);
  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();


  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use("EnvironmentProgram");
  GLuint pid = shader->getProgramID("EnvironmentProgram");

  glUseProgram(pid);

  glm::mat4 M, MV, MVP;
  glm::mat3 N;

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
