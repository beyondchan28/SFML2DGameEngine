#include "Physics.h"
#include <math.h>
#include <iostream>
#include "Vec2.h"

Physics::Physics()
{}

bool Physics::isCircleIntersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float sqrDeltaPos = std::pow((x1 - x2), 2) + std::pow((y1 - y2),2);
    float sqrRad = std::pow((r1 + r2), 2);
    return sqrDeltaPos <= sqrRad;
}

Vec2 Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) const
{

    if (a->hasComponent<CBoundingBox>() && b->hasComponent<CBoundingBox>())
    {
        Vec2 overlap = {0, 0};
        Vec2 & aHalfSize= a->getComponent<CBoundingBox>().halfSize;
        Vec2 & bHalfSize= b->getComponent<CBoundingBox>().halfSize;

        Vec2 & aPos = a->getComponent<CTransform>().pos;
        Vec2 & bPos = b->getComponent<CTransform>().pos;

        float dx = std::abs(aPos.x - bPos.x);
        float dy = std::abs(aPos.y - bPos.y);

        float ox = aHalfSize.x + bHalfSize.x - dx;
        float oy = aHalfSize.y + bHalfSize.y - dy;

        overlap = Vec2(ox, oy);

    }

}



Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    return Vec2(0,0);
}

bool Physics::isVerticalOverlap(float overlapY)
{
    if (overlapY > 0.0f)
    {
        return true;
    }

    else
    {
        return false;
    }
}
