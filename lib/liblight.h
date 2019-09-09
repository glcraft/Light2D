#include <glm/glm.hpp>
#include <memory>
#include <map>

#define MAX_NUM_TOTAL_LIGHTS 10
#define MAX_NUM_TOTAL_WALLS 10

namespace li
{
    namespace shader
    {
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
            int numOfLights=nb_lights;
        };
        template<int nb_lights, int nb_walls>
        struct Walls
        {
            Wall walls[nb_walls];
            WallTangent walltangs[nb_walls*nb_lights];
            int numOfWalls = nb_walls;
        };
    }
    using glm::vec2;
    using glm::vec3;
    class Light
    {
    public:
        Light(vec2 pos=vec2(0.f), vec3 col=vec3(1.f,1.f,1.f), float size=0.0f, float strength=1.0f);
        void setPosition(vec2 _pos)         { m_position =_pos; }
        vec2 getPosition()                  { return m_position; }
        void setSize(float _size)           { m_size = _size; }
        float getSize()                     { return m_size; }
        void setStrength(float _strength)   { m_strength = _strength; }
        float getStrength()                 { return m_strength; }
        void setColor(vec3 _col)            { m_color =_col; }
        vec3 getColor()                     { return m_color; }
        shader::Light getShaderLight();
    protected:
        vec2 m_position=vec2(0.f);
        vec3 m_color;
        float m_size=0.f, m_strength=1.f;
    };
    class Wall
    {
    public:
        Wall(vec2 _pos1=vec2(0.f), vec2 _pos2=vec2(0.f));
        void setPositions(vec2 _pos1, vec2 _pos2)   { m_pos1=_pos1; m_pos2=_pos2; }
        glm::vec2 getPoint1() { return m_pos1; }
        glm::vec2 getPoint2() { return m_pos2; }
        vec2 getDirection() { return glm::normalize(m_pos2-m_pos1); }
        vec2 getNormal();
        shader::Wall getShaderWall();
        shader::WallTangent getShaderWallInfo();
    protected:
        vec2 m_pos1, m_pos2;
        // float m_width;
    };
    class Manager
    {
    public:
        using ID = std::size_t;
        ID addLight(Light&& light)
        { 
            auto t = m_lights.data.insert({m_lights.currentID++, std::make_unique<Light>(light)});
            m_update=true;
            return t.first->first;
        }
        ID addWall(Wall&& wall)
        { 
            auto t = m_walls.data.insert({m_walls.currentID++, std::make_unique<Wall>(wall)});
            m_update=true;
            return t.first->first; 
        }
        Light& getLight(ID id)
        {
            return *m_lights.data.at(id);
        }
        Wall& getWall(ID id)
        {
            return *m_walls.data.at(id);
        }
        const shader::Lights<10> & getLightsShader() { return m_shadLights; }
        const shader::Walls<10,10> & getWallsShader() { return m_shadWalls; }

        bool hasUpdateData() { return m_update; }
        void forceUpdateData() { m_update = true; }
        bool updateData();

    protected:
        template<typename Obj>
        struct Container
        {
            std::map<ID, std::unique_ptr<Obj>> data;
            ID currentID = 1;
        };
        Container<Light> m_lights;
        Container<Wall> m_walls;
        shader::Lights<10> m_shadLights;
        shader::Walls<10,10> m_shadWalls;
        bool m_update;
    };
}