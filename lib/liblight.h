#include <glm/glm.hpp>
#include <vector>
namespace li
{
    using glm::vec2;
    class Light
    {
    public:
        void setPosition(vec2 _pos) { m_position =_pos; }
        vec2 getPosition()          { return m_position; }
        void setSize(float _size)   { m_size = _size; }
        float getSize()             { return m_size; }
    protected:
        vec2 m_position=vec2(0.f);
        float m_size=0.f;
    };
    class Wall
    {
    public:
        void setPositions(vec2 _pos1, vec2 _pos2)   { m_pos1=_pos1; m_pos2=_pos2; }
        vec2 getDirection() { return glm::normalize(m_pos2-m_pos1); }
        vec2 getNormal();
    protected:
        vec2 m_pos1, m_pos2;
        // float m_width;
    };
    class Manager
    {
    public:
    protected:
        std::vector<Light> m_lights;
        std::vector<Wall> m_walls;
    };
}