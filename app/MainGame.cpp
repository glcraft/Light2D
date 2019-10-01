#include "MainGame.h"
#include <filesystem>
#include <random>
#include <chrono>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "JsonToValue.hpp"

#define NBBLOCK 50
#define SIZE_TERRAIN 30
#define TEST_NWALLS 1
#define TEST_NCORNER 4



void MainGame::init()
{
    namespace fs = std::filesystem;
    
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, SIZE_TERRAIN-1);

    m_input.createWindow("Light2D", SDL_WINDOW_OPENGL, glm::ivec2(SDL_WINDOWPOS_CENTERED),  glm::ivec2(1024));
    SDL_GL_SetSwapInterval(1);
    glewExperimental = true;
    glewInit();

    glxinfo();

    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
    try
    {
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
    }
    catch(gl::sl::CompileException exc)
    {std::cout <<"Compile Exception"<< exc.what(); std::cin.get(); exit(1);}
    catch(gl::sl::Program::LinkException exc)
    {std::cout <<"Link Exception"<< exc.what(); std::cin.get(); exit(1);}

    struct MatGL
    {
        glm::mat4 view, proj;
    };
    gl::UniformBuffer<MatGL> uni_matrices;
    uni_matrices.instantiate();
    uni_matrices.reserve(1);
    uni_matrices.setName("Matrices");
    
    auto matgl = uni_matrices.map_write();
    matgl->view = glm::mat4(1.f);
    matgl->proj=glm::ortho<float>(0, SIZE_TERRAIN, SIZE_TERRAIN, 0, -1, 1);
    uni_matrices.unmap();
    
    uni_lights.instantiate();
    uni_walls.instantiate();

    uni_lights.reserve(1);
    uni_lights.setName("Lights");
    uni_walls.reserve(1);
    uni_walls.setName("Walls");
    
    {
        using namespace glm;
        load_json();

        m_managerLight.updateData();

        auto lights = uni_lights.map_write();
        auto walls = uni_walls.map_write();
        memcpy(lights, &m_managerLight.getLightsShader(), sizeof(m_managerLight.getLightsShader()));
        memcpy(walls, &m_managerLight.getWallsShader(), sizeof(m_managerLight.getWallsShader()));
        uni_walls.unmap();
        uni_lights.unmap();
        
    }
    uni_matrices.bindBase(1);
    uni_lights.bindBase(2);
    uni_walls.bindBase(3);
    uni_matrices.bind(m_program.terrain, m_program.blockBlack, m_program.screen);
    uni_walls.bind(m_program.screen);
    uni_lights.bind(m_program.screen);
    

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
    glClearColor(1,1,1,1);
    auto time0 = std::chrono::steady_clock::now();

    glViewport(0,0,m_input.getWindowData().size.x, m_input.getWindowData().size.y);
    
    while(!quit)
    {
        std::chrono::duration<float, std::ratio<1,1>> current_time(std::chrono::steady_clock::now()-time0);
        Input::update();
        if (m_input.getKeyPressed(SDL_SCANCODE_ESCAPE)||m_input.getWindowData().closed)
            quit=true;
        
        glClear(GL_COLOR_BUFFER_BIT);
        m_program.screen << gl::sl::use
            << gl::UniformStatic<float>("time", current_time.count());
        // uni_walls.bind(m_program.screen);
        // uni_lights.bind(m_program.screen);
        
        m_fbotex.bindTo(0);
        m_vboScreen.draw(GL_TRIANGLE_FAN);
        m_fbotex.unbind();
        m_input.getWindowData().swapBuffers();
    }
}
#define GLDISP_INT(name) glGetIntegerv(name, &iv); std::cout << #name ": " << iv << std::endl;
void MainGame::glxinfo()
{
    int iv;
    std::cout << "OpenGL Info\n";
    GLDISP_INT(GL_MAX_UNIFORM_BLOCK_SIZE)
    GLDISP_INT(GL_MAX_UNIFORM_BUFFER_BINDINGS)
    GLDISP_INT(GL_MAX_UNIFORM_LOCATIONS)
    std::cout << "\n\n";
}
void MainGame::updateLiInfo()
{
    if (m_managerLight.updateData())
    {
        auto lights = uni_lights.map_write();
        auto walls = uni_walls.map_write();
        memcpy(lights, &m_managerLight.getLightsShader(), sizeof(m_managerLight.getLightsShader()));
        memcpy(walls, &m_managerLight.getWallsShader(), sizeof(m_managerLight.getWallsShader()));
        uni_walls.unmap();
        uni_lights.unmap();
    }
}


void MainGame::load_json()
{
    namespace nl = nlohmann;
    nl::json jsFile;
    std::ifstream ifstr("light.json");
    if (ifstr)
        ifstr >> jsFile;
    for (auto& jLight : jsFile["lights"])
    {
        JSONToValue<glm::vec2> pos(0.f);
        JSONToValue<glm::vec3> color(1.f);
        JSONToValue<float> size(1.f);
        JSONToValue<float> strength(1.f);
        pos.set(jLight["position"]);
        color.set(jLight["color"]);
        size.set(jLight["size"]);
        strength.set(jLight["strenght"]);

        m_managerLight.addLight(li::Light(pos, color, size, strength));
    }
    for (auto& jWall : jsFile["walls"])
    {
        JSONToValue<glm::vec2> pos1(0.f);
        JSONToValue<glm::vec2> pos2(0.f);
        pos1.set(jWall["position1"]);
        pos2.set(jWall["position2"]);
        m_managerLight.addWall(li::Wall(pos1, pos2));
    }
}