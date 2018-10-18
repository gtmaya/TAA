#ifndef USERCAMERA_H
#define USERCAMERA_H

#include <ngl/Obj.h>
#include <glm/glm.hpp>

class UserCamera
{
public:
    UserCamera();
    void handleMouseMove(const double _xpos, const double _ypos);
    void handleMouseClick(const double _xpos, const double _ypos, const int _button, const int _action, const int _mods);
    void handleKey(const int _key, const bool _state);
    void resize(const int _width, const int _height);
    void update();
    glm::mat4 viewMatrix() const;
    glm::mat4 projMatrix() const;


private:
};

#endif
