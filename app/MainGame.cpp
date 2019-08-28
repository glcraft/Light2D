#include "MainGame.h"
#include <filesystem>
#include <random>
#include <chrono>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#define NBBLOCK 50
#define SIZE_TERRAIN 30

inline glm::vec2 get_normal(glm::vec2 dir)
{return glm::vec2(dir.y, -dir.x);}
Dir setDir(glm::vec2 d)
{
    glm::vec2 n = normalize(d);
    return Dir{n, get_normal(n)};
}
inline glm::vec2 getXY(glm::vec4 v)
{return glm::vec2(v.x, v.y);}
inline glm::vec2 getZW(glm::vec4 v)
{return glm::vec2(v.z, v.w);}
glm::vec4 finding_tangent(glm::vec2 center_circle, float radius, glm::vec2 point)
{
    glm::vec2 d = center_circle - point;
    float dd = length(d);
    float a = glm::asin(radius / dd);
    float b = glm::atan(d.y, d.x);
    
    float ta = b - a, tb = b + a;
    glm::vec4 pts=radius * glm::vec4(glm::sin(ta), -glm::cos(ta), -glm::sin(tb), glm::cos(tb));
    return glm::vec4((center_circle+getXY(pts)-point), (center_circle+getZW(pts)-point));
}
WallInfo setWallInfo(glm::vec4 wall, glm::vec2 posLight, float size)
{
    WallInfo wi;
    wi.pointLeft = glm::vec2(wall.x, wall.y);
    wi.pointRight =  glm::vec2(wall.z, wall.w);
    wi.direction=setDir(wi.pointRight-wi.pointLeft);
    
    if (size>0)
    {
        glm::vec4 tan1 = finding_tangent(posLight, size, wi.pointLeft), tan2 = finding_tangent(posLight, size, wi.pointRight);
        wi.innerLeft = setDir(getXY(tan1));
        wi.innerRight = setDir(getZW(tan2));
        wi.outerLeft = setDir(getZW(tan1));
        wi.outerRight = setDir(getXY(tan2));
    }
    else
    {
        wi.innerLeft = wi.outerLeft = setDir(posLight-wi.pointLeft);
        wi.innerRight = wi.outerRight = setDir(posLight-wi.pointRight);
    }
    return wi;
}

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
    
    const int nblights=3;
    const int nbwalls=2;
    {
        auto lights = uni_lights.map_write();
        auto walls = uni_walls.map_write();
        float size=0.05f, strength=0.5f;
        float top=0.7f;
        const float spacing = 0.25;
        const glm::vec2 posLight[3]={glm::vec2(0.5f-spacing, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f+spacing, 0.5f) };
        const glm::vec3 colLight[3]={glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1)};
        const glm::vec4 _wall[2]={glm::vec4(0.2,top, 0.45,top), glm::vec4(0.55,top, 0.8,top)};
        for (int iLight=0;iLight<nblights;++iLight)
        {
            lights->lights[iLight] = Light{posLight[iLight], glm::vec2(size, strength), glm::vec4(colLight[iLight],1.)};
            for (int iWall = 0;iWall<nbwalls;++iWall)
                walls->walls[iWall+iLight*nbwalls] = setWallInfo(_wall[iWall], lights->lights[iLight].position, lights->lights[iLight].size_strength.x);
        }
        lights->numOfLights=nblights;
        walls->numOfWalls=nbwalls;
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
    glm::mat4 prmat(0.1f);
    prmat[3][3]=1;
    gl::UniformStatic<glm::mat4> uni_viewmat("viewmat", glm::mat4(1.f)), uni_projmat("projmat", glm::ortho<float>(0, SIZE_TERRAIN, SIZE_TERRAIN, 0, -1, 1));
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