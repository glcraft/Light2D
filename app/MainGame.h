#pragma once
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Input.h"
#include <liblight.h>

#define MAX_NUM_TOTAL_LIGHTS 10
#define MAX_NUM_TOTAL_WALLS 10

struct Dir
{
    glm::vec2 line;
    glm::vec2 normal;
};
struct Wall
{
    glm::vec2 pointLeft, pointRight;
    Dir direction;
};
struct WallTangent
{
    Dir innerLeft, innerRight;
    Dir outerLeft, outerRight;
};
struct Light
{
    glm::vec2 position;
    glm::vec2 size_strength;
    glm::vec4 color;
};
template<int nb_lights>
struct Lights
{
    Light lights[nb_lights];
    int numOfLights=nb_light;
};
template<int nb_lights, int nb_walls>
struct Walls
{
    Wall walls[nb_walls];
    WallTangent walltangs[nb_walls*nb_lights];
    int numOfWalls = nb_walls;
};
// template<int nb_lights, int nb_walls>
// struct Uniform_Liblight
// {
//     Lights<nb_lights> lights;
//     Walls<nb_lights, nb_walls> lights;
// };

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
    gl::UniformBuffer<Lights<10>> uni_lights;
    gl::UniformBuffer<Walls<10, 10>> uni_walls;
};