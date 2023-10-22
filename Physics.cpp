#include "Physics.h"
#include <math.h>
#include <cmath>
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
    Vec2 overlap = {0, 0};

    if (a->hasComponent<CBoundingBox>() && b->hasComponent<CBoundingBox>())
    {
        Vec2 & aOrigin= a->getComponent<CBoundingBox>().halfSize;
        Vec2 & bOrigin= b->getComponent<CBoundingBox>().halfSize;

        Vec2 & aPos = a->getComponent<CTransform>().pos;
        Vec2 & bPos = b->getComponent<CTransform>().pos;

        float aXBBox = aPos.x + aOrigin.x;
        float aYBBox = aPos.y + aOrigin.y;
        float bXBBox = bPos.x + bOrigin.x;
        float bYBBox = bPos.y + bOrigin.y;

        float dx = std::abs(aXBBox - bXBBox);
        float dy = std::abs(aYBBox - bYBBox);

        float xAxisOverlap = aXBBox + bXBBox - dx;
        float yAxisOverlap = aYBBox + bYBBox - dy;

        std::cout << xAxisOverlap << "\n";
        std::cout << yAxisOverlap << "\n";

        //if both is positive : means overlap
        if (xAxisOverlap > 0 || yAxisOverlap > 0)
        {
            std::cout << "Overlap" << "\n";
            overlap.x = xAxisOverlap;
            overlap.y = yAxisOverlap;
        }
    }

    return overlap;
}

Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    return Vec2(0,0);
}
