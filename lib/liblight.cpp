#include "liblight.h"
namespace li
{
    inline glm::vec2 get_normal(glm::vec2 dir)
    {return glm::vec2(dir.y, -dir.x);}
    shader::Dir setDir(glm::vec2 d)
    {
        glm::vec2 n = normalize(d);
        return shader::Dir{n, get_normal(n)};
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
    shader::WallTangent setWallInfo(shader::Wall& wall, glm::vec2 posLight, float size)
    {
        shader::WallTangent wi;
        glm::vec2 left = wall.pointLeft, right=wall.pointRight;
        if (size>0)
        {
            glm::vec4 tan1 = finding_tangent(posLight, size, left), tan2 = finding_tangent(posLight, size, right);
            wi.innerLeft = setDir(getXY(tan1));
            wi.innerRight = setDir(getZW(tan2));
            wi.outerLeft = setDir(getZW(tan1));
            wi.outerRight = setDir(getXY(tan2));
        }
        else
        {
            wi.innerLeft = wi.outerLeft = setDir(posLight-left);
            wi.innerRight = wi.outerRight = setDir(posLight-right);
        }
        return wi;
    }

    Light::Light(vec2 pos, vec3 col, float size, float strength) : m_position(pos), m_color(col), m_size(size), m_strength(strength)
    {

    }
    Wall::Wall(vec2 _pos1, vec2 _pos2) : m_pos1(_pos1), m_pos2(_pos2)
    {

    }

    shader::Wall Wall::getShaderWall()
    {
        shader::Wall w;
        w.pointLeft = m_pos1;
        w.pointRight = m_pos2;
        w.direction = setDir(w.pointRight - w.pointLeft);
        return w;
    }

    shader::Light Light::getShaderLight()
    {
        return shader::Light{m_position, glm::vec2(m_size, m_strength), glm::vec4(m_color,1.)}; // posLight[iLight], glm::vec2(size, strength), glm::vec4(colLight[iLight],1.)};
    }

    bool Manager::updateData()
    {
        if (!m_update)
            return false;
        ID posWall=0;
        for(const auto& [id, wall] : m_walls.data)
            m_shadWalls.walls[posWall++] = wall->getShaderWall();
        posWall=0;
        ID posLight=0, posWallLight=0;
        for(const auto& [idLight, light] : m_lights.data)
        {
            m_shadLights.lights[posLight] = light->getShaderLight();
            posWall=0;
            for(const auto& [idWall, wall] : m_walls.data)
            {
                m_shadWalls.walltangs[posWallLight++] = setWallInfo(m_shadWalls.walls[posWall], m_shadLights.lights[posLight].position, m_shadLights.lights[posLight].size_strength.x);
                ++posWall;
            }
            ++posLight;
        }
        m_shadLights.numOfLights=m_lights.data.size();
        m_shadWalls.numOfWalls=m_walls.data.size();
        return true;
    }
}