#include "MainGame.h"
#	undef main
int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    MainGame m_main;
    m_main.init();
    m_main.display();
    SDL_Quit();
    return 0;
}