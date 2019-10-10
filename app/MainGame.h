#pragma once
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Input.h"
#include <liblight.h>
#include "JsonToValue.hpp"

class MainGame
{
public:
    void init();
    void display();
private:
    void glxinfo();
    void updateLiInfo();
    void load_json();
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

    li::Manager m_managerLight;
    std::vector<std::unique_ptr<jsonexpr::AbstractValue>> m_tJsexpr;
    float m_time;
};