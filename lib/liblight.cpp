#include "liblight.h"
namespace li
{
    vec2 Wall::getNormal() 
    { 
        auto vecDir = getDirection();
        return vec2(vecDir.y, -vecDir.x);
    };
}