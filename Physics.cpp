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

        Vec2 bPos = b->getComponent<CTransform>().pos;
        Vec2 bHalfSize = b->getComponent<CBoundingBox>().halfSize;

        Vec2 overlapDir = {0,0};

        if (aPos.y < bPos.y - bHalfSize.y)
        {
            overlapDir.y = -1;
        }
        else if (aPos.y > bPos.y + bHalfSize.y)
        {
            overlapDir.y = 1;
        }
        else if (aPos.x < bPos.x - bHalfSize.x)
        {
            overlapDir.x = -1;
        }
        else if (aPos.x > bPos.x + bHalfSize.x)
        {
            overlapDir.x = 1;
        }

//        std::cerr << overlapDir.x << " " << overlapDir.y << "\n";

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

float Physics::approach(float goal, float current, float dt)
{
    float diff = goal - current;

    if (diff > dt)
    {
        return current + dt;
    }
    if (diff < -dt)
    {
        return current - dt;
    }

    return goal;
}

//bool Scene::checkBoxTrap()
//{
//    auto & pPos = m_entityManager.getEntities("Player")[0]->getComponent<CTransform>().pos;
//    sf::View view = m_game->window().getView();
//    const sf::Vector2f & windowCenter = view.getCenter();
//
//    float halfSize = 50.f;
////        float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x );
////        float windowCenterY = std::max(m_game->window().getSize().y / 2.0f, pPos.y );
//
//    Vec2 topLeft = {windowCenter.x - halfSize, windowCenter.y - halfSize};
//    Vec2 topRight = {windowCenter.x + halfSize, windowCenter.y - halfSize};
//    Vec2 botLeft = {windowCenter.x - halfSize, windowCenter.y + halfSize};
//    Vec2 botRight = {windowCenter.x + halfSize, windowCenter.y + halfSize};
//
//    drawLine(topLeft, botLeft);
//    drawLine(topLeft, topRight);
//    drawLine(topRight, botRight);
//    drawLine(botLeft, botRight);
//
//    if(pPos.x < topLeft.x && pPos.x < botLeft.x)
//    {
//        if(pPos.y > topLeft.y && pPos.y < botLeft.y)
//        {
//            return true;
//        }
//    }
//    else if(pPos.x > topRight.x && pPos.x > botRight.x)
//    {
//        if(pPos.y > topRight.y && pPos.y < botRight.y)
//        {
//            return true;
//
//        }
//    }
//    else if(pPos.y < topLeft.y && pPos.y < topRight.y)
//    {
//        if(pPos.x > topLeft.x && pPos.x < topRight.x)
//        {
//            return true;
//
//        }
//    }
//
//    else if(pPos.y > botLeft.y && pPos.y > botRight.y)
//    {
//        if(pPos.x > botLeft.x && pPos.x < botRight.x)
//        {
//            return true;
//
//        }
//    }
//
//    return false;
//
//}

