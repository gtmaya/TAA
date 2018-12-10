#include "renderscene.h"
#include "usercamera.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>

RenderScene r_scene;
UserCamera r_camera;

void error_callback(int error, const char* description)
{
  std::cerr << "Error ("<<error<<"): " << description << "\n";
}

void cursor_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
  if (r_camera.cursorActive()) {r_camera.handleMouseMove(xpos, ypos);}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  r_camera.handleMouseClick(xpos, ypos, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  r_camera.handleScroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
{
  static bool curstate = true;
  // Escape exits the application
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
        r_scene.setAAMethod(RenderScene::msaa);
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

  // Set our OpenGL version
  //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
  //glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  int width = 1000; int height = 1000;
  GLFWwindow* window = glfwCreateWindow(width,
                                        height,
                                        "Temporal Anti Aliasing Demo",
                                        nullptr,
                                        nullptr);


  if (window == nullptr)
  {
    glfwTerminate();
  }

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
