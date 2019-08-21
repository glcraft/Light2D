#include "MainGame.h"
#include <GL/glew.h>

#define NBBLOCK 5
void MainGame::init()
{
    m_input.createWindow("Light2D");
    m_vbo.attachVertexArray(std::make_shared<gl::VertexArray>());
    m_vbo.reserve(NBBLOCK*6);
    glm::vec2* values = m_vbo.map_write();
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
    m_vbo.unmap();
    m_vbo.set_attrib(decltype(m_vbo)::Attrib<0>(0,2,GL_FLOAT, GL_FALSE));
}
void MainGame::display()
{
    bool quit=false;
    while(!quit)
    {
        Input::update();
        if (m_input.getKeyPressed(SDL_SCANCODE_ESCAPE)||m_input.getWindowData().closed)
            quit=true;
        
        glClear(GL_COLOR_BUFFER_BIT);

        m_input.getWindowData().swapBuffers();
    }
}