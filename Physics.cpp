#include "Physics.h"
#include <math.h>

Physics::Physics()
{}

bool Physics::isCircleIntersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float sqrDeltaPos = std::pow((x1 - x2), 2) + std::pow((y1 - y2),2);
    float sqrRad = std::pow((r1 + r2), 2);
    return sqrDeltaPos <= sqrRad;
}
