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
    Vec2 overlap = {0, 0};

    if (a->hasComponent<CBoundingBox>() && b->hasComponent<CBoundingBox>())
    {
        Vec2 & aOrigin= a->getComponent<CBoundingBox>().halfSize;
        Vec2 & bOrigin= b->getComponent<CBoundingBox>().halfSize;

        Vec2 & aPos = a->getComponent<CTransform>().pos;
        Vec2 & bPos = b->getComponent<CTransform>().pos;

        float dx = std::abs((aPos.x + aOrigin.x) - (bPos.x + bOrigin.x));
        float dy = std::abs((aPos.y + aOrigin.y) - (bPos.y + bOrigin.y));

        bool xOverlap = dx <= (aOrigin.x + bOrigin.x);
        bool yOverlap = dy <= (aOrigin.y + bOrigin.y);

        //std::cout << dx << "\n";
        //std::cout << dy << "\n";

        //if both is positive : means overlap
        if (xOverlap && yOverlap)
        {

            std::cout << "Overlap" << "\n";
            //overlap.x = xAxisOverlap;
            //overlap.y = yAxisOverlap;
        }
        else
        {
            std::cout << "No Overlap" << "\n";

        }

    }

    return overlap;
}

Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    return Vec2(0,0);
}
