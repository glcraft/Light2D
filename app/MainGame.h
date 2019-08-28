#pragma once
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Input.h"


namespace gl
{
    //http://www.geeks3d.com/3dfr/20140703/uniform-buffers-objects-opengl-31-tutorial/
    template<typename MyStruct>
    class UniformBuffer : public Buffer<GL_UNIFORM_BUFFER, MyStruct>
    {
    public:
        using BufferBase = Buffer<GL_UNIFORM_BUFFER, MyStruct>;
        UniformBuffer() : BufferBase()
		{
			
		}
        ~UniformBuffer()
		{
			this->destroy();
		}
        void setName(std::string_view block_name)
        {
            m_blockName = block_name;
        }
        
        void bindBase(GLuint bind_point)
        {
            m_bindPoint = bind_point;
            BufferBase::bind();
            glBindBufferBase(GL_UNIFORM_BUFFER, m_bindPoint, id());
        }
        template <typename ...Args>
        void bind(Args... programs)
        {
            (bind(programs), ...);
        }
        void bind(gl::sl::Program& program)
        {
            BufferBase::bind();
            int blockIndx= getBlockIndex(program, m_blockName);
            
            if (blockIndx!=GL_INVALID_INDEX)
            {
                glUniformBlockBinding(program.id(), blockIndx, m_bindPoint);
            }
        }
    protected:
        GLuint getBlockIndex(gl::sl::Program& program, std::string_view block_name)
        {
            return glGetUniformBlockIndex(program.id(), block_name.data());
        }
    private:
        GLuint m_bindPoint=0;
        std::string m_blockName;
    };
}

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
    Light lights[MAX_NUM_TOTAL_LIGHTS];
    int numOfLights=nb_light;
};
template<int nb_lights, int nb_walls>
struct Walls
{
    Wall walls[MAX_NUM_TOTAL_WALLS];
    WallTangent walltangs[MAX_NUM_TOTAL_WALLS*MAX_NUM_TOTAL_LIGHTS];
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