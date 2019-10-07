#include <glm/glm.hpp>
#include <memory>
#include <map>

#define MAX_NUM_TOTAL_LIGHTS 10
#define MAX_NUM_TOTAL_WALLS 10

namespace jsonexpr
{template <typename T> class Value;}

namespace li
{
    namespace shader
    {
        using namespace glm;
        struct Dir
        {
            vec2 line;
            vec2 normal;
        };
        struct Wall
        {
            vec2 pointLeft, pointRight;
            Dir direction;
        };
        struct WallTangent
        {
            vec2 inner[2];
            vec2 outer[2];
            ivec4 sens;
        };
        struct Light
        {
            vec2 position;
            vec2 size_strength;
            vec4 color;
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
    class ShapeUpdate
    {
    public:
        void setUpdater (std::shared_ptr<bool> updater){ m_updater = updater; *m_updater = true; }
    protected:
        void updateShape(){if (m_updater) *m_updater=true; }
        std::shared_ptr<bool> m_updater;
    };
    class Light : public ShapeUpdate
    {
    public:
        Light(vec2 pos=vec2(0.f), vec3 col=vec3(1.f,1.f,1.f), float size=0.0f, float strength=1.0f);
        void setPosition(vec2 _pos)         { m_position =_pos; updateShape(); }
        vec2 getPosition()                  { return m_position; }
        void setSize(float _size)           { m_size = _size; updateShape(); }
        float getSize()                     { return m_size; }
        void setStrength(float _strength)   { m_strength = _strength; updateShape(); }
        float getStrength()                 { return m_strength; }
        void setColor(vec3 _col)            { m_color =_col; updateShape(); }
        vec3 getColor()                     { return m_color; }
        shader::Light getShaderLight();
    protected:
        friend class jsonexpr::Value<Light>;
        vec2 m_position=vec2(0.f);
        vec3 m_color;
        float m_size=0.f, m_strength=1.f;
    };
    class Wall : public ShapeUpdate
    {
    public:
        Wall(vec2 _pos1=vec2(0.f), vec2 _pos2=vec2(0.f));
        void setPositions(vec2 _pos1, vec2 _pos2)   { m_pos1=_pos1; m_pos2=_pos2; updateShape(); }
        glm::vec2 getPoint1() { return m_pos1; }
        glm::vec2 getPoint2() { return m_pos2; }
        vec2 getDirection() { return glm::normalize(m_pos2-m_pos1); }
        vec2 getNormal();
        shader::Wall getShaderWall();
        shader::WallTangent getShaderWallInfo();
    protected:
        friend class jsonexpr::Value<Wall>;
        vec2 m_pos1, m_pos2;
        // float m_width;
    };
    class Manager
    {
    public:
        Manager();
        using ID = std::size_t;
        ID addLight(Light&& light);
        ID addWall(Wall&& wall);
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

        bool hasUpdateData() { return *m_update; }
        void forceUpdateData() { *m_update = true; }
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
        std::shared_ptr<bool> m_update;
    };
}