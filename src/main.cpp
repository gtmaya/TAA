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
/// @file main.cpp
/// @brief Main file handling camera and scene updates.  Modified and adapted from Richard Southern's Demo on
/// Phong Shading available at https://github.com/NCCA/rendering_examples
//---------------------------------------------------------------------------------------------------------------
#include "renderscene.h"
#include "usercamera.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
/// @brief Scene object that contains TAA routines.
RenderScene r_scene;
/// @brief Camera object that manages the camera and its matrices.
UserCamera r_camera;
//------------------------------------------------------------------------------------------------------------------------------
// GLFW Callbacks
// These functions are triggered on an event, such as a keypress
// or mouse click. They need to be passed on to the relevant
// handler, for example, the camera or scene.
//------------------------------------------------------------------------------------------------------------------------------
/// @brief Function to catch GLFW errors.
/// @param error GLFW error code
/// @param description Text description
//------------------------------------------------------------------------------------------------------------------------------
void error_callback(int error, const char* description)
{
  std::cerr << "Error ("<<error<<"): " << description << "\n";
}
//------------------------------------------------------------------------------------------------------------------------------
/// @brief Function to catch GLFW cursor movement
/// @param xpos x position
/// @param ypos y position
//------------------------------------------------------------------------------------------------------------------------------
void cursor_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
  if (r_camera.cursorActive()) {r_camera.handleMouseMove(xpos, ypos);}
}
//------------------------------------------------------------------------------------------------------------------------------
/// @brief Handle a mouse click or release
/// @param window Window handle (unused currently)
/// @param button Which button was pressed (e.g. left or right button)
/// @param action GLFW code for the action (GLFW_PRESS or GLFW_RELEASE)
/// @param mods Other keys which are currently being held down (e.g. GLFW_CTRL)
//------------------------------------------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  r_camera.handleMouseClick(xpos, ypos, button, action, mods);
}
//------------------------------------------------------------------------------------------------------------------------------
/// @brief Handle key press or release
/// @param xoffset How far has been scrolled in the x dimension
/// @param yoffset How far has been scrolled in the y dimension
//------------------------------------------------------------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  r_camera.handleScroll(xoffset, yoffset);
}
//------------------------------------------------------------------------------------------------------------------------------
/// @brief Handle key press or release
/// @param window window handle (unused currently)
/// @param key The key that was pressed
/// @param action GLFW code for the action (GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT)
/// @param mods Other keys which are currently being held down (e.g. GLFW_CTRL)
//------------------------------------------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
{
  static bool curstate = true;
  if (action == GLFW_PRESS)
  {
    switch(key)
    {
      case (GLFW_KEY_ESCAPE):
        glfwSetWindowShouldClose(window, true); break;
      case (GLFW_KEY_SPACE):
      {
        r_camera.toggleCursorState();
        curstate = !curstate;
        if (curstate)
        {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        break;
      }
      case (GLFW_KEY_ENTER):
      {
        r_camera.reset();
        r_scene.resetTAA();
        break;
      }
      case (GLFW_KEY_1):
      {
        r_scene.setAAMethod(RenderScene::taa);
        break;
      }
      case (GLFW_KEY_2):
      {
        r_scene.setAAMethod(RenderScene::none);
        break;
      }
      case (GLFW_KEY_3):
      {
        r_scene.setAAMethod(RenderScene::noPass);
        break;
      }
      case (GLFW_KEY_KP_2):
      {
        r_scene.increaseFeedback(-0.01f);
        break;
      }
      case (GLFW_KEY_KP_8):
      {
        r_scene.increaseFeedback(0.01f);
        break;
      }
    }
  }
  r_camera.handleKey(key, action);
}
//------------------------------------------------------------------------------------------------------------------------------
/// @brief Handle a window resize event.
/// @param width New window width
/// @param height New window height
//------------------------------------------------------------------------------------------------------------------------------
void resize_callback(GLFWwindow */*window*/, int width, int height)
{
  r_camera.resize(width,height);
  r_scene.resizeGL(width,height);
}

int main()
{
  if (!glfwInit())
  {
    // Initialisation failed
    glfwTerminate();
  }
  // Register error callback
  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
//  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  int width = 1000; int height = 1000;
  GLFWwindow* window = glfwCreateWindow(width, height, "Temporal Anti Aliasing Demo", nullptr, nullptr);

  if (window == nullptr) {glfwTerminate();}

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  // Disable the cursor for the FPS camera
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set the mouse move and click callback
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  r_scene.initGL();
  std::cout<<"\n\n\nControls:\n"
             "               forward   W\n"
             "            left/right A   S\n"
             "              backward   D \n"
             "               down/up Q   E\n"
             "\n"
             "                  SPACE\n"
             "         toggle cursor window lock\n"
             "\n"
             "                  ENTER\n"
             "          reset camera position\n"
             "              1 - Toggle AA\n"
             "         2 - Reset AA accumulation\n"
             "\n"
             "        numpad 8 - increase feedback\n"
             "        numpad 2 - decrease feedback\n"
             "\n"
             "\n";

  // Set the window resize callback and call it once
  glfwSetFramebufferSizeCallback(window, resize_callback);
  resize_callback(window, width, height);

  // Main render loop
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    r_camera.update();
    r_scene.setViewMatrix(r_camera.viewMatrix());
    r_scene.setProjMatrix(r_camera.projMatrix());
    r_scene.setCubeMatrix(r_camera.cubeMatrix());
    r_scene.setCameraLocation(r_camera.getLocation());
    r_scene.paintGL();
    glfwSwapBuffers(window);
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}
