#pragma once
#include "Vec2.h"

//physics class is to find at what point the both entity collides when the overlap is happening
class Physics
{
public:
    Physics();
    //Vec2 CirlcleGetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    bool isCircleIntersect(float x1, float y1, float r1, float x2, float y2, float r2);
};
