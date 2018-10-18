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
  r_camera.handleMouseMove(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  r_camera.handleMouseClick(xpos, ypos, button, action, mods);
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
{
  static bool curstate = false;
  // Escape exits the application
  if (action == GLFW_PRESS)
  {
    switch(key)
    {
      case (GLFW_KEY_ESCAPE):
        glfwSetWindowShouldClose(window, true); break;
      case (GLFW_KEY_SPACE):
      {
        curstate = !curstate;
        if (curstate)
        {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
      }
    }
  }
  r_camera.handleKey(key, (action == GLFW_PRESS));
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  int width = 1200; int height = 720;
  GLFWwindow* window = glfwCreateWindow(width,
                                        height,
                                        "Basic Render Scene",
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

  r_scene.initGL();

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

    r_scene.paintGL();

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
