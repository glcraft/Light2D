#pragma once
#include <libglw/GLClass.h>
#include "Input.h"
class MainGame
{
public:
    void init();
    void display();
private:
    Input m_input;
    gl::ArrayBuffer<glm::vec2> m_vbo;
};