#pragma once
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Input.h"

#define MAX_NUM_TOTAL_LIGHTS 10
#define MAX_NUM_TOTAL_WALLS 10

struct Dir
{
    glm::vec2 line;
    glm::vec2 normal;
};
struct WallInfo
{
    glm::vec2 pointLeft, pointRight;
    Dir innerLeft, innerRight;
    Dir outerLeft, outerRight;
    Dir direction;
};
struct Light
{
    glm::vec2 position;
    glm::vec3 color;
    float size, strenght;
};

struct Lights
{
    Light lights[MAX_NUM_TOTAL_LIGHTS];
    int numOfLights;
};
struct Walls
{
    WallInfo walls[MAX_NUM_TOTAL_WALLS];
    int numOfWalls;
};

class MainGame
{
public:
    void init();
    void display();
private:
    void glxinfo();
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
    gl::UniformBuffer<Lights> uni_lights;
    gl::UniformBuffer<Walls> uni_walls;
};