#include "MainGame.h"
#include <filesystem>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#define NBBLOCK 5
void MainGame::init()
{
    namespace fs = std::filesystem; 
    
    m_input.createWindow("Light2D", SDL_WINDOW_OPENGL);
    glewExperimental = true;
    glewInit();

    m_vbo = std::make_unique<VBOType>();
    m_vbo->reserve(NBBLOCK*6);
    glm::vec2* values = m_vbo->map_write();
    size_t offset=0;
    for (int i=0;i<NBBLOCK;++i)
    {
        glm::vec2 pos(rand()%10, rand()%10);
        values[offset++]=pos+glm::vec2(0,0);
        values[offset++]=pos+glm::vec2(1,0);
        values[offset++]=pos+glm::vec2(0,1);
        values[offset++]=pos+glm::vec2(1,0);
        values[offset++]=pos+glm::vec2(1,1);
        values[offset++]=pos+glm::vec2(0,1);
    }
    m_vbo->unmap();
    m_vbo->attachVertexArray(std::make_shared<gl::VertexArray>());
    m_vbo->set_attrib(VBOType::Attrib<0>(0, 2, GL_FLOAT, GL_FALSE));

    m_program
        << gl::sl::Shader<gl::sl::Vertex>(fs::path("res/shaders/terrain.vert"))
        << gl::sl::Shader<gl::sl::Fragment>(fs::path("res/shaders/terrain.frag"))
        << gl::sl::link;
}
void MainGame::display()
{
    bool quit=false;
    glm::mat4 prmat(0.1f);
    prmat[3][3]=1;
    gl::UniformStatic<glm::mat4> uni_viewmat("viewmat", glm::mat4(1.f)), uni_projmat("projmat", prmat);//glm::ortho<float>(-20, 20, 20, -20, -1, 1)
    glClearColor(1,1,1,1);
    while(!quit)
    {
        Input::update();
        if (m_input.getKeyPressed(SDL_SCANCODE_ESCAPE)||m_input.getWindowData().closed)
            quit=true;
        
        glClear(GL_COLOR_BUFFER_BIT);
        m_program << gl::sl::use
            << uni_viewmat
            << uni_projmat;
        m_vbo->draw(GL_TRIANGLES);

        m_input.getWindowData().swapBuffers();
    }
}