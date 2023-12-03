#pragma once
#include "Vec2.h"
#include "Entity.h"
#include <memory>
#include <queue>

//physics class is to find at what point the both entity collides when the overlap is happening
class Physics
{
public:
    struct Intersect { bool result; Vec2 pos;};

    Physics();
    //Vec2 CirlcleGetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    bool isCircleIntersect(float x1, float y1, float r1, float x2, float y2, float r2);
    bool isOverlap(Vec2 overlap);
    bool isPointInCircle(Vec2 circlePos, float circleRadius, Vec2 pointPos) const;

    Vec2 getOverlap(std::shared_ptr<Entity> a, const  std::shared_ptr<Entity> b) const;
    //previous overlap uses entity's previous position ?
    Vec2 getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    Vec2 getOverlapDirection(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

    Intersect lineIntersection(Vec2 a, Vec2 b, Vec2 c, Vec2 d);

    float approach(float goal, float current, float dt);
};
