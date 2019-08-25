#include "MainGame.h"
#include <filesystem>
#include <random>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#define NBBLOCK 7
#define SIZE_TERRAIN 20
void MainGame::init()
{
    namespace fs = std::filesystem;
    
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, SIZE_TERRAIN-1);

    m_input.createWindow("Light2D", SDL_WINDOW_OPENGL,  glm::ivec2(SDL_WINDOWPOS_CENTERED),  glm::ivec2(1024));
    glewExperimental = true;
    glewInit();
    
    gl::Object::SetAutoInstantiate(true);

    m_vboScreen.instantiate();
    m_vboScreen.reserve(4);
    auto vertScreen = m_vboScreen.map_write();
    vertScreen[0]=glm::vec2(0);
    vertScreen[1]=glm::vec2(0,1);
    vertScreen[2]=glm::vec2(1);
    vertScreen[3]=glm::vec2(1,0);
    m_vboScreen.unmap();
    m_vboScreen.attachVertexArray(std::make_shared<gl::VertexArray>());
    m_vboScreen.set_attrib(VBOType::Attrib<0>(0, 2, GL_FLOAT, GL_FALSE));

    m_vbo = std::make_unique<VBOType>();
    m_vbo->reserve(NBBLOCK*6);
    glm::vec2* values = m_vbo->map_write();
    size_t offset=0;
    for (int i=0;i<NBBLOCK;++i)
    {
        glm::vec2 pos(dis(gen), dis(gen));
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

    m_program.terrain
        << gl::sl::Shader<gl::sl::Vertex>(fs::path("res/shaders/terrain.vert"))
        << gl::sl::Shader<gl::sl::Fragment>(fs::path("res/shaders/terrain.frag"))
        << gl::sl::link;
    m_program.blockBlack
        << gl::sl::Shader<gl::sl::Vertex>(fs::path("res/shaders/blockBlack.vert"))
        << gl::sl::Shader<gl::sl::Fragment>(fs::path("res/shaders/blockBlack.frag"))
        << gl::sl::link;
    m_program.screen
        << gl::sl::Shader<gl::sl::Vertex>(fs::path("res/shaders/screen.vert"))
        << gl::sl::Shader<gl::sl::Fragment>(fs::path("res/shaders/screen.frag"))
        << gl::sl::link;
    // m_fbo = std::make_unique<gl::Framebuffer>();
    auto sampl = std::make_shared<gl::Sampler>();
    // sampl->instantiate();
    sampl->setFiltering(gl::Sampler::Linear);
    sampl->setWrap(gl::Sampler::ClampToBorder);
    m_fbotex.instantiate();
    m_fbotex.setSampler(sampl);
    m_fbotex.setSize(glm::vec2(SIZE_TERRAIN));
    m_fbotex.setTarget(GL_TEXTURE_2D);
    m_fbotex.setFormat(GL_RGBA);
    m_fbotex.init_null(GL_RGBA, GL_UNSIGNED_BYTE);
    m_fbo.instantiate();
    m_fbo.attachTexture(gl::Framebuffer::AttachColor0, m_fbotex, 0);
    m_fbo.BindScreen();
}
void MainGame::display()
{
    bool quit=false;
    glm::mat4 prmat(0.1f);
    prmat[3][3]=1;
    gl::UniformStatic<glm::mat4> uni_viewmat("viewmat", glm::mat4(1.f)), uni_projmat("projmat", glm::ortho<float>(0, SIZE_TERRAIN, SIZE_TERRAIN, 0, -1, 1));
    glClearColor(1,1,1,1);
    while(!quit)
    {
        Input::update();
        if (m_input.getKeyPressed(SDL_SCANCODE_ESCAPE)||m_input.getWindowData().closed)
            quit=true;
        
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0,0,SIZE_TERRAIN,SIZE_TERRAIN);
        m_fbo.bind();
        m_program.blockBlack << gl::sl::use
            << uni_viewmat
            << uni_projmat;
        m_vbo->draw(GL_TRIANGLES);
        m_fbo.BindScreen();
        glViewport(0,0,m_input.getWindowData().size.x, m_input.getWindowData().size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        m_program.screen << gl::sl::use
            << gl::UniformStatic<int>("tex0", 0);
        m_fbotex.bindTo(0);
        m_vboScreen.draw(GL_TRIANGLE_FAN);
        m_fbotex.unbind();

        m_input.getWindowData().swapBuffers();
    }
}