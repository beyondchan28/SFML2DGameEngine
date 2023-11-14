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

        return overlap = Vec2(ox, oy);

    }

}



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

Vec2 Physics::getOverlapDirection(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // calculate after the overlap happen
    if (a->hasComponent<CBoundingBox>() && b->hasComponent<CBoundingBox>())
    {
        Vec2 & aPrevPos = a->getComponent<CTransform>().prevPos;
        Vec2 & bPrevPos = b->getComponent<CTransform>().prevPos;

        Vec2 & aHalfSize = a->getComponent<CBoundingBox>().halfSize;
        Vec2 & bHalfSize = b->getComponent<CBoundingBox>().halfSize;

//        Vec aTopRightCorner = {aPos.x + aHalfSize.x, aPos.y - aHalfSize.y};
//        Vec aTopLeftCorner = {aPos.x - aHalfSize.x, aPos.y - aHalfSize.y};
//        Vec aBotLeftCorner = {aPos.x - aHalfSize.x, aPos.y + aHalfSize.y};
//        Vec aBotRightCorner= {aPos.x + aHalfSize.x, aPos.y + aHalfSize.y};
//
//        Vec bTopRightCorner = {bPos.x + bHalfSize.x, bPos.y - bHalfSize.y};
//        Vec bTopLeftCorner = {bPos.x - bHalfSize.x, bPos.y - bHalfSize.y};//        Vec bBotLeftCorner = {bPos.x - bHalfSize.x, bPos.y + bHalfSize.y};
//        Vec bBotRightCorner= {bPos.x + bHalfSize.x, bPos.y + bHalfSize.y};

//        Vec2 aTop = (aPos.x, aPos.y - aHalfSize.y);
//        Vec2 aBot = (aPos.x, aPos.y + aHalfSize.y);
//        Vec2 aLeft = (aPos.x - aHalfSize.x, aPos.y);
//        Vec2 aRight = (aPos.x + aHalfSize.x, aPos.y);
//
//        Vec2 bTop = (bPos.x, bPos.y - bHalfSize.y);
//        Vec2 bBot = (bPos.x, bPos.y + bHalfSize.y);
//        Vec2 bLeft = (bPos.x - bHalfSize.x, bPos.y);
//        Vec2 bRight = (bPos.x + bHalfSize.x, bPos.y);

//        float aTop = aPos.y - aHalfSize.y;
//        float aBot =  aPos.y + aHalfSize.y;
//        float aLeft = aPos.x - aHalfSize.x;
//        float aRight = aPos.x + aHalfSize.x;
//
//        float bTop = bPos.y - bHalfSize.y;
//        float bBot = bPos.y + bHalfSize.y;
//        float bLeft = bPos.x - bHalfSize.x;
//        float bRight = bPos.x + bHalfSize.x;

//        bool isAOnTopB = (aBot > bTop);
//        bool isAOnBotB = (aTop < bBot);
//        bool isAOnLeftB = (aRight > bLeft);
//        bool isAOnRightB = (aLeft < bRight;


        //this is a of b direction
//        if(aPrevPos)
//        {
//            std::cout << "TOP" << "\n";
//            return Vec2(0, -1); // a at the left of b
//        }
//        else if(isAOnBotB)
//        {
//            std::cout << "BOT" << "\n";
//            return Vec2(0, 1); // a at the left of b
//        }
//        else if(isAOnLeftB)
//        {
//            std::cout << "LEFT" << "\n";
//            return Vec2(-1, 0);  // a at the left of b
//        }
//        else if(isAOnRightB)
//        {
//            std::cout << "RIGHT" << "\n";
//            return Vec2(1, 0); // a at the left of b
//        }

    }

}


bool Physics::isOverlap(Vec2 overlap)
{
    return (overlap.x > 0.0f && overlap.y > 0.0f)? true : false;
}


