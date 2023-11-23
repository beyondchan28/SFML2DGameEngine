#include "Physics.h"
#include <math.h>
#include <iostream>
#include "Vec2.h"

Physics::Physics(){}

bool Physics::isCircleIntersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float sqrDeltaPos = std::pow((x1 - x2), 2) + std::pow((y1 - y2),2);
    float sqrRad = std::pow((r1 + r2), 2);
    return sqrDeltaPos <= sqrRad;
}

bool Physics::isPointInCircle(Vec2 circlePos, float circleRadius, Vec2 pointPos) const
{
    return pointPos.x > circlePos.x - circleRadius && pointPos.x < circlePos.x + circleRadius &&
           pointPos.y > circlePos.y - circleRadius && pointPos.y < circlePos.y + circleRadius;
}

bool Physics::isOverlap(Vec2 overlap)
{
    return (overlap.x > 0.0f && overlap.y > 0.0f)? true : false;
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

        return overlap = Vec2(ox, oy);

    }

}


// still unused, might deleted later
Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
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

        return overlap = Vec2(ox, oy);

    }
}

// calculate the collision direction from A to B
// example : A (player) and B (tile)

// the bug is because if y velocity = 2, it takes 2 pixel to finally colliding
// so, before it took 2 pixel to collide which is 1 pixel more than it used to
// detect the direction
Vec2 Physics::getOverlapDirection(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    if (a->hasComponent<CBoundingBox>() && b->hasComponent<CBoundingBox>())
    {
        Vec2 aPos = a->getComponent<CTransform>().pos;
        Vec2 aHalfSize = a->getComponent<CBoundingBox>().halfSize;

        Vec2 bPos = b->getComponent<CTransform>().pos;
        Vec2 bHalfSize = b->getComponent<CBoundingBox>().halfSize;

        Vec2 overlapDir = {0,0};

        float aTopLeftX = aPos.x - aHalfSize.x;
        float aTopLeftY = aPos.y - aHalfSize.y;

        float aBotRightX = aPos.x + aHalfSize.x;
        float aBotRightY = aPos.y + aHalfSize.y;

        float bTopLeftX = bPos.x - bHalfSize.x;
        float bTopLeftY = bPos.y - bHalfSize.y;

        float bBotRightX = bPos.x + bHalfSize.x;
        float bBotRightY = bPos.y + bHalfSize.y;

        // there is something wrong on this. its not that efficient. not that good
        float dYTB = std::abs(aTopLeftY - bBotRightY);
        float dYTT = std::abs(aTopLeftY - bTopLeftY);

        float dYBB = std::abs(aBotRightY - bBotRightY);
        float dYBT = std::abs(aBotRightY - bTopLeftY);

        float dXTT = std::abs(aTopLeftX - bTopLeftX);
        float dXTB = std::abs(aTopLeftX - bBotRightX);

        float dXBT = std::abs(aBotRightX - bTopLeftX);
        float dXBB = std::abs(aBotRightX - bBotRightX);

        bool isTop = dYTT > dYBT;
        bool isBot = dYTB < dYBB;
        bool isLeft = dXBT < dXTT;
        bool isRight= dXBB > dXTB;

        if(isRight) { overlapDir = {1, 0}; }
        else if(isLeft) { overlapDir = {-1, 0}; }
        else if(isBot) { overlapDir = {0, 1}; }
        else if(isTop) { overlapDir = {0, -1}; }

        return overlapDir;
    }

}


Physics::Intersect Physics::lineIntersection(Vec2 a, Vec2 b, Vec2 c, Vec2 d)
{
    Vec2 r = (b - a);
    Vec2 s = (d - c);
    float rxs = r.cross(s);
    Vec2 cma = c - a;
    float t = (cma.cross(s)) / rxs;
    float u = (cma.cross(r)) / rxs;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        Vec2 intersectPoint(a.x + t*r.x, a.y + t*r.y);

        struct Physics::Intersect intersect;
        intersect.result = true;
        intersect.pos = intersectPoint;

        return intersect;
    }
    else
    {
        struct Physics::Intersect intersect;
        intersect.result = false;
        intersect.pos = Vec2(0,0);

        return intersect;
    }



}

