#include "MainGame.h"
#include <GL/glew.h>
void MainGame::init()
{
    m_input.createWindow("Light2D");
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