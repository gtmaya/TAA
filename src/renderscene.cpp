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
/// @file renderscene.cpp
/// @brief Handles rendering of the scene, and contains main TAA routine.
//---------------------------------------------------------------------------------------------------------------
#include "renderscene.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <ngl/Obj.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <iomanip>
//---------------------------------------------------------------------------------------------------------------
/// @brief Basic constructor initialising the scene and its clocks for frame rate calculation.
//---------------------------------------------------------------------------------------------------------------
RenderScene::RenderScene() : m_width(1),
                             m_height(1),
                             m_ratio(1.0f),
                             m_startTime (std::chrono::high_resolution_clock::now()),
                             m_prevFrameTime (std::chrono::high_resolution_clock::now())
{}

RenderScene::~RenderScene() = default;

//---------------------------------------------------------------------------------------------------------------
/// @brief Function to update the scene's width and height and its aspect ratio.  Sets flag for FBOs to be
/// re-initialised, and updates the size of pixels in UV coordinates so that the jitter vectors can be resized
/// accordingly.
/// @param _width Width input
/// @param _height Height input
//---------------------------------------------------------------------------------------------------------------
void RenderScene::resizeGL(GLint _width, GLint _height) noexcept
{
  m_width = _width;
  m_height = _height;
  m_ratio = m_width / float(m_height);
  m_isFBODirty = true;
  m_pixelSizeScreenSpace.x = 1.f / m_width;
  m_pixelSizeScreenSpace.y = 1.f / m_height;
  updateJitter();
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to initialise the GL context for the scene - must be called before any draw calls.  Also
/// creates VAOs for imported objs, compiles shaders and calls for textures to be initialised.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::initGL() noexcept
{
  ngl::NGLInit::instance();
//  glClearColor(0.8f, 0.8f, 0.8f, 1.f);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  m_arrObj[0].m_mesh = new ngl::Obj("models/plane.obj");
  m_arrObj[1].m_mesh = new ngl::Obj("models/torus.obj");
  m_arrObj[2].m_mesh = new ngl::Obj("models/platonic.obj");
  m_arrObj[3].m_mesh = new ngl::Obj("models/cube.obj");
  m_arrObj[4].m_mesh = new ngl::Obj("models/text.obj");
  m_arrObj[5].m_mesh = new ngl::Obj("models/gear.obj");
  m_arrObj[0].m_shaderProps.m_diffuseTex = taa_checkerboard;
  m_arrObj[0].m_shaderProps.m_diffuseWeight = 0.25f;
  m_arrObj[0].m_shaderProps.m_specularWeight = 0.25f;
  m_arrObj[0].m_shaderProps.m_roughness = 1.f;

  for (auto &i : m_arrObj)
  {
    i.m_mesh->createVAO();
  }

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->loadShader("beckmannShader",
                     "shaders/beckmann_v.glsl",
                     "shaders/beckmann_f.glsl");

  shader->loadShader("taaShader",
                     "shaders/taa_v.glsl",
                     "shaders/taa_f.glsl");

  shader->loadShader("blitShader",
                     "shaders/blit_v.glsl",
                     "shaders/blit_f.glsl");

  initEnvironment();
  initTexture(taa_checkerboard, m_checkerboardTex, "images/checkerboard.jpg");
  initTexture(taa_dirt, m_dirtTex, "images/dirt.jpg");

  shader->use("beckmannShader");
  GLuint shaderID = shader->getProgramID("beckmannShader");

  glUniform3fv(glGetUniformLocation(shaderID, "lightCol"),
               int(m_lightCol.size()),
               glm::value_ptr(m_lightCol[0]));

  glUniform3fv(glGetUniformLocation(shaderID, "lightPos"),
               int(m_lightCol.size()),
               glm::value_ptr(m_lightPos[0]));

  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",2,2,1,1,ngl::Vec3(0,1,0));
  m_pixelSizeScreenSpace.x = 1.f / m_width;
  m_pixelSizeScreenSpace.y = 1.f / m_height;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function that renders the scene and runs the TAA pass.  Other passes could be added into this loop
/// e.g. motion blur, depth of field, sharpening etc.  This funciton switches the FBO for the anti-aliased result
/// to be drawn to each frame so that the previous resulting anti-aliased frame can be sent to the TAA shader as
/// a feedback loop.  First the scene is rendered with a beckmann PBR shader, then, if TAA is active, the TAA
/// pass is run.  If TAA is off, the scene is either rendered directly to the screen, or to an FBO texture and
/// then blit to the screen.
/// The jitter is also cycled once per frame here, and the FPS is calculated and output to the terminal every 2
/// seconds.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::paintGL() noexcept
{
  static int count = 0;
  //Common stuff
  if (m_isFBODirty)
  {
    initRenderFBO();
    initAAFBO(m_aaFBO1, m_aaFBOColour1, m_aaFBODepth1);
    initAAFBO(m_aaFBO2, m_aaFBOColour2, m_aaFBODepth2);
    m_aaDirty = true;
    m_isFBODirty = false;
  }

  size_t activeAAFBO;
  if (m_flip) {activeAAFBO = m_aaFBO1;}
  else        {activeAAFBO = m_aaFBO2;}

  //Scene
  beckmannRender(activeAAFBO);


  if (m_activeAA != noPass)
  {
    //AA
    if (!m_aaDirty && m_activeAA == taa) {antialias(activeAAFBO);}

    //Blit
    if (m_flip)
    {
//      blit(m_aaFBO1, m_aaFBOColour1, m_aaColourTU1);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, m_aaFBOColour1);
      glDrawBuffer(GL_BACK);
      glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    else
    {
//      blit(m_aaFBO2, m_aaFBOColour2, m_aaColourTU2);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, m_aaFBOColour2);
      glDrawBuffer(GL_BACK);
      glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    //Cycle jitter
    m_jitterCounter++;
    if (m_jitterCounter > (m_sampleVector.size() - 1)) {m_jitterCounter = 0;}

    m_aaDirty = false;
    m_flip = !m_flip;
  }

  //Calculate fps
  auto now = std::chrono::high_resolution_clock::now();
  int elapsedSeconds = int(std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count());
  static bool startSecond = true;
  if (elapsedSeconds % 2 == 1)
  {
    if (startSecond)
    {
      double fps = count / 2.0;
      std::cout<<fps<<'\t'<<" FPS\n";
      startSecond = false;
      count = 0;
    }
  }
  else {startSecond = true;}
  count++;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief This function is the TAA pass, which binds the FBO for the result of the AA to be rendered to, along
/// with sending the correct uniforms to the TAA shader, such as the previously anti-aliased frame, the jitter
/// and so on.
/// @param _activeAAFBO The identifier of the FBO for the AA result to be rendered to.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::antialias(size_t _activeAAFBO)
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[_activeAAFBO][taa_fboID]);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  glm::mat4 screenMVP = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f,0.0f,0.0f));

  shader->use("taaShader");
  GLuint shaderID = shader->getProgramID("taaShader");

  glm::mat4 inverseVPC = glm::inverse(m_VP);

  glActiveTexture(m_renderFBOColour);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboTextureID]);
  glActiveTexture(m_renderFBODepth);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboDepthID]);
  //Bind the inactive aaFBO
  if (_activeAAFBO == m_aaFBO1)
  {
    glActiveTexture(m_aaFBOColour2);
    glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_aaFBO2][taa_fboTextureID]);
  }
  else
  {
    glActiveTexture(m_aaFBOColour1);
    glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_aaFBO1][taa_fboTextureID]);
  }
  glUniform1i(glGetUniformLocation(shaderID, "velocityBUF"), m_renderVelTU);
  glUniform1i(glGetUniformLocation(shaderID, "colourRENDER"), m_renderColourTU);
  glUniform1i(glGetUniformLocation(shaderID, "depthRENDER"), m_renderDepthTU);
  if (_activeAAFBO == m_aaFBO1) {glUniform1i(glGetUniformLocation(shaderID, "colourANTIALIASED"),  m_aaColourTU2);}
  else                          {glUniform1i(glGetUniformLocation(shaderID, "colourANTIALIASED"),  m_aaColourTU1);}
  glUniform2f(glGetUniformLocation(shaderID, "windowSize"), m_width, m_height);
  glUniformMatrix4fv(glGetUniformLocation(shaderID, "inverseViewProjectionCURRENT"),
                     1,
                     false,
                     glm::value_ptr(inverseVPC));
  glUniformMatrix4fv(glGetUniformLocation(shaderID, "viewProjectionHISTORY"),
                     1,
                     false,
                     glm::value_ptr(m_lastVP));
  glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"),
                     1,
                     false,
                     glm::value_ptr(screenMVP));
  glUniformMatrix4fv(glGetUniformLocation(shaderID, "invJitter"),
                     1,
                     false,
                     glm::value_ptr(m_invJitter));
  glm::vec2 screenSpaceJitter = m_jitterVector[m_jitterCounter] * -0.5f;
  glUniform2fv(glGetUniformLocation(shaderID, "jitter"),
               1,
               glm::value_ptr(screenSpaceJitter));
  glUniform1f(glGetUniformLocation(shaderID, "feedback"), m_feedback);
  glUniform2fv(glGetUniformLocation(shaderID, "pixelSize"),
               1,
               glm::value_ptr(m_pixelSizeScreenSpace));
  prim->draw("plane");
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Debug function for testing FBO textures (used to visualise recent clamping events stored in the alpha
/// channel).
/// @param _fbo The FBO to render to the screen
/// @param _texture The GL Texture to be blitted
/// @param _textureUnit The texture unit of said texture
//---------------------------------------------------------------------------------------------------------------
void RenderScene::blit(size_t _fbo, GLenum _texture, int _textureUnit)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  glm::mat4 screenMVP = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f,0.0f,0.0f));

  shader->use("blitShader");
  GLuint shaderID = shader->getProgramID("blitShader");

  glActiveTexture(_texture);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[_fbo][taa_fboTextureID]);

  glUniform1i(glGetUniformLocation(shaderID, "inputTex"), _textureUnit);
  glUniform2f(glGetUniformLocation(shaderID, "windowSize"), m_width, m_height);
  glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"),
                     1,
                     false,
                     glm::value_ptr(screenMVP));
  prim->draw("plane");
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to render all objects in the scene with a beckmann based PBR shader.  This renders to the
/// given FBO unless the user has signified this should be rendered directly to the screen (if m_activeAA is
/// noPass, which is if the user presses '3' on the number row.
/// @param _activeAAFBO Used for any frame when there is no 'clean' history buffer for TAA to use - in this case
/// we render to the buffer that will be the history buffer the next frame so that TAA has some data to use.
/// (Thus technically not used for the majority of frames).
//---------------------------------------------------------------------------------------------------------------
void RenderScene::beckmannRender(size_t _activeAAFBO)
{
  if (m_aaDirty || m_activeAA == none) {glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[_activeAAFBO][taa_fboID]);}
  else if (m_activeAA == taa)          {glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[m_renderFBO][taa_fboID]);}
  else                                 {glBindFramebuffer(GL_FRAMEBUFFER, 0);}
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  GLuint shaderID = shader->getProgramID("beckmannShader");
  shader->use("beckmannShader");

  m_lastVP = m_VP;
  glm::mat4 jitterMatrix;
  if (m_activeAA == taa)
  {
    glm::vec3 help {m_jitterVector[m_jitterCounter].x, m_jitterVector[m_jitterCounter].y, 0.f};
    jitterMatrix = glm::translate(jitterMatrix, help);
  }
  //Jitter the VP
  m_VP = jitterMatrix * m_proj * m_view;
  m_invJitter = glm::inverse(jitterMatrix);

  glUniform1i(glGetUniformLocation(shaderID, "envMapMaxLod"), 10);
  glUniform3fv(glGetUniformLocation(shaderID, "cameraPos"),
               1,
               glm::value_ptr(m_cameraPos));

  for (auto &obj : m_arrObj)
  {
    glm::mat4 M, MV, MVP;
    glm::mat3 N;
    M = glm::mat4(1.f);
    M = glm::rotate(M, obj.rotation[0], glm::vec3(1.f, 0.f, 0.f));
    M = glm::rotate(M, obj.rotation[1], glm::vec3(0.f, 1.f, 0.f));
    M = glm::rotate(M, obj.rotation[2], glm::vec3(0.f, 0.f, 1.f));
    M = glm::translate(M, obj.position);
    static bool isFirstFrame = true;
    if (isFirstFrame) {obj.previousMVP = M;}
    isFirstFrame = false;
    MV = m_view * M;
    MVP = m_VP * M;
    glm::mat4 MVPNoJitter = m_proj * m_view * M;
    N = glm::inverse(glm::mat3(M));

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"),
                       1,
                       false,
                       glm::value_ptr(MV));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVPNJPrevious"),
                       1,
                       false,
                       glm::value_ptr(obj.previousMVP));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVPNoJitter"),
                       1,
                       false,
                       glm::value_ptr(MVPNoJitter));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"),
                       1,
                       false,
                       glm::value_ptr(MVP));
    glUniformMatrix3fv(glGetUniformLocation(shaderID, "N"),
                       1,
                       true,
                       glm::value_ptr(N));
    glUniform1f(glGetUniformLocation(shaderID, "roughness"), obj.m_shaderProps.m_roughness);
    glUniform1f(glGetUniformLocation(shaderID, "metallic"), obj.m_shaderProps.m_metallic);
    glUniform1f(glGetUniformLocation(shaderID, "diffAmount"), obj.m_shaderProps.m_diffuseWeight);
    glUniform1f(glGetUniformLocation(shaderID, "specAmount"), obj.m_shaderProps.m_specularWeight);
    glUniform3fv(glGetUniformLocation(shaderID, "materialDiff"),
                 1,
                 glm::value_ptr(obj.m_shaderProps.m_diffuseColour));
    glUniform3fv(glGetUniformLocation(shaderID, "materialSpec"),
                 1,
                 glm::value_ptr(obj.m_shaderProps.m_specularColour));
    glUniform1f(glGetUniformLocation(shaderID, "alpha"), obj.m_shaderProps.m_alpha);
    if (obj.m_shaderProps.m_diffuseTex == taa_checkerboard)
    {
      glUniform1i(glGetUniformLocation(shaderID, "hasDiffMap"), 1);
      glUniform1i(glGetUniformLocation(shaderID, "diffuseMap"), taa_checkerboard);
    }
    else if (obj.m_shaderProps.m_diffuseTex == taa_dirt)
    {
      glUniform1i(glGetUniformLocation(shaderID, "hasDiffMap"), 1);
      glUniform1i(glGetUniformLocation(shaderID, "diffuseMap"), taa_dirt);
    }
    else
    {
      glUniform1i(glGetUniformLocation(shaderID, "hasDiffMap"), 0);
    }
    obj.m_mesh->draw();
    obj.update();
    obj.previousMVP = MVPNoJitter;
  }
  //Remove the jitter
  m_VP = m_proj * m_view;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Sets the view matrix to an input given by the camera in the main render loop.
/// @param _view The incoming view matrix.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::setViewMatrix(glm::mat4 _view)
{
  m_lastView = m_view;
  m_view = _view;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Sets the projection matrix to an input given by the camera in the main render loop.
/// @param _view The incoming projection matrix.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::setProjMatrix(glm::mat4 _proj)
{
  m_lastProj = m_proj;
  m_proj = _proj;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Sets the view matrix for an environment cube to an input given by the camera in the main render loop.
/// @param _view The incoming environment cube view matrix.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::setCubeMatrix(glm::mat4 _cube)
{
  m_cube = _cube;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Sets the position of the camera.
/// @param _location The incoming camera position matrix.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::setCameraLocation(glm::vec3 _location)
{
  m_cameraPos = _location;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Sets the view matrix to an input given by the camera in the main render loop.  Originally planned to
/// implement switch to MSAA but this was not possible due to how GLFW handles MSAA, as the window would need to
/// be killed and a new one made with MSAA active.
/// @param _method The incoming method to be used (TAA, none (render to texture, blit texture to screen) or
/// noPass (render straight to screen).
//---------------------------------------------------------------------------------------------------------------
void RenderScene::setAAMethod(int _method)
{
  m_activeAA = _method;
  if (m_activeAA != noPass) {m_aaDirty = true;}
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Tags whether the anti alising would be 'dirty'.  This would be the case if the window has been
/// resized, if the AA method has been changed (as long as there *is* something to be dirty) etc.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::resetTAA()
{
  if (m_activeAA != noPass) {m_aaDirty = true;}
}
//---------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
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
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use("environmentShader");
  shader->setUniform("envMap", 0);
  shader->use("beckmannShader");
  shader->setUniform("envMap", 0);
}
//---------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
void RenderScene::initEnvironmentSide(GLenum _target, const char *_filename)
{
    ngl::Image img(_filename);
    glTexImage2D(_target,
                 0,
                 int(img.format()),
                 int(img.width()),
                 int(img.height()),
                 0,
                 img.format(),
                 GL_UNSIGNED_BYTE,
                 img.getPixels());
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function used to initialise (or re-initialise) the FBO that is used for the rendered frame.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::initRenderFBO()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[m_renderFBO][taa_fboID]);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == (GL_FRAMEBUFFER_COMPLETE))
  {
   glDeleteTextures(1, &m_arrFBO[m_renderFBO][taa_fboTextureID]);
   glDeleteTextures(1, &m_arrFBO[m_renderFBO][taa_fboDepthID]);
   glDeleteTextures(1, &m_arrFBO[m_renderFBO][taa_fboVelID]);
   glDeleteFramebuffers(1, &m_arrFBO[m_renderFBO][taa_fboID]);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenTextures(1, &m_arrFBO[m_renderFBO][taa_fboTextureID]);
  glActiveTexture(m_renderFBOColour);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboTextureID]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &m_arrFBO[m_renderFBO][taa_fboDepthID]);
  glActiveTexture(m_renderFBODepth);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboDepthID]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &m_arrFBO[m_renderFBO][taa_fboVelID]);
  glActiveTexture(m_renderFBOVel);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboVelID]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glGenFramebuffers(1, &m_arrFBO[m_renderFBO][taa_fboID]);
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[m_renderFBO][taa_fboID]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboTextureID], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboVelID], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_arrFBO[m_renderFBO][taa_fboDepthID], 0);

  GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, drawBufs);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {std::cout<<"Help\n";}
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function used to initialise (or re-initialise) an FBO used for TAA.
/// @param _fboID The ID signifying which of the two AA FBOs we are initialising
/// @param _textureA The GL texture to be used as the colour attachment
/// @param _textureB The GL texture to be used as the depth attachment
//---------------------------------------------------------------------------------------------------------------
void RenderScene::initAAFBO(size_t _fboID, GLenum _textureA, GLenum _textureB)
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &m_arrFBO[_fboID][taa_fboDepthID]);
  glActiveTexture(_textureB);
  glBindTexture(GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboDepthID]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenFramebuffers(1, &m_arrFBO[_fboID][taa_fboID]);
  glBindFramebuffer(GL_FRAMEBUFFER, m_arrFBO[_fboID][taa_fboID]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboTextureID], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_arrFBO[_fboID][taa_fboDepthID], 0);

  GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBufs);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {std::cout<<"Help\n";}
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Function to initialise a texture to be used later as a 2D sampler in a shader.
/// @param _texUnit The texture unit for this texture to be stored in
/// @param _texID The ID for this texture
/// @param _filename The path to the texture to be initialised
//---------------------------------------------------------------------------------------------------------------
void RenderScene::initTexture(const GLuint& _texUnit, GLuint& _texID, const char* _filename)
{
    glActiveTexture(GL_TEXTURE0 + _texUnit);
    ngl::Image img(_filename);
    glGenTextures(1, &_texID);
    glBindTexture(GL_TEXTURE_2D, _texID);
    glTexImage2D (GL_TEXTURE_2D, 0, int(img.format()), int(img.width()), int(img.height()), 0, img.format(), GL_UNSIGNED_BYTE, img.getPixels());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Updates the jitter samples that are used for the TAA jitter to be resolution dependant, so that we're
/// always using a sub pixel jitter.
//---------------------------------------------------------------------------------------------------------------
void RenderScene::updateJitter()
{
  for (size_t i = 0; i < m_jitterVector.size(); i++){m_jitterVector[i] = m_sampleVector[i] * m_pixelSizeScreenSpace * 0.9f;}
}
//---------------------------------------------------------------------------------------------------------------
/// @brief Changes the feedback amount by a delta (which can be positive or negative) but clamps it between 0 and
/// 1.
/// @param _delta The amount we are increasing or decreasing the feedback by
//---------------------------------------------------------------------------------------------------------------
void RenderScene::increaseFeedback(float _delta)
{
  m_feedback += _delta;
  if (m_feedback > 1.f) {m_feedback = 1.f;}
  if (m_feedback < 0.f) {m_feedback = 0.f;}
  std::cout<<"Keeping "<<m_feedback * 100.f<<"% of the current frame and "<<(1 - m_feedback) * 100.f<<"% of the previous frame\n";
}
