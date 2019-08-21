#pragma once
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Input.h"
class MainGame
{
public:
    void init();
    void display();
private:
    using VBOType = gl::ArrayBuffer<glm::vec2>;
    Input m_input;
    gl::sl::Program m_program;
    VBOType::uptr m_vbo;
};