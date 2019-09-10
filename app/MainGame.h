#pragma once
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Input.h"
#include <liblight.h>


class MainGame
{
public:
    void init();
    void display();
private:
    void glxinfo();
    void updateLiInfo();
    using VBOType = gl::ArrayBuffer<glm::vec2>;
    Input m_input;
    struct _Shader
    {
        gl::sl::Program terrain, blockBlack, screen;
    } m_program;
    // VBOType::uptr m_vbo;
    VBOType m_vboScreen;
    gl::Framebuffer m_fbo;
    gl::Texture m_fbotex;
    gl::UniformBuffer<li::shader::Lights<10>> uni_lights;
    gl::UniformBuffer<li::shader::Walls<10, 10>> uni_walls;

    li::Manager::ID m_IDwall[3];
    li::Manager m_managerLight;
};