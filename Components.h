#pragma once

#include "SFML/Graphics.hpp"
#include "Vec2.h"
#include "Animation.h"

class Component
{
public:
    bool has = false;
};

class CTransform : public Component
{
public:
    Vec2 pos = {0.0, 0.0};
    Vec2 prevPos = {0.0, 0.0};
    Vec2 scale = {1.0, 1.0};
    Vec2 velocity = {0.0, 0.0};
    float angle = 0;

    CTransform() {}
    CTransform(const Vec2 & p)
        : pos(p)
    {}
    CTransform(const Vec2 & p, const Vec2 & vel, const Vec2 & sc, float a)
        :pos(p), prevPos(p), velocity(vel), scale(sc), angle(a)
    {}
};

class CRaycast : public Component
{
public:
    Vec2 pointA = {0.0 , 0.0};
    Vec2 pointB = {0.0 , 0.0};

    CRaycast() {};
    CRaycast(const Vec2 & pA, const Vec2 & pB)
    : pointA(pA), pointB(pB)
    {};
};

class CShape : public Component
{
public:
    sf::CircleShape circle;

    CShape(){}
    CShape(float radius, size_t points, const sf::Color & fill, const sf::Color & outline, float thickness)
        : circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }
};

class CRectangle : public Component
{
public:
    sf::RectangleShape rectangle;

    CRectangle(){}
    CRectangle(Vec2 rectSize)
        : rectangle(sf::Vector2f(rectSize.x, rectSize.y))
    {
        rectangle.setOutlineColor(sf::Color::White);
        rectangle.setFillColor(sf::Color(0,0,0,0));
        rectangle.setOutlineThickness(1);
        rectangle.setOrigin(rectSize.x / 2.0f, rectSize.y / 2.0f);
    }
};


class CCollision : public Component
{
public:
    float radius = 0;
    CCollision(){}
    CCollision(float rad)
        :radius(rad)
    {}

};

class CScore : public Component
{
public:
    int score = 0;

    CScore(){}
    CScore(int s)
        :score(s)
    {}
};


class CBoundingBox : public Component
{
public:
    Vec2 size;
    Vec2 halfSize;
    CBoundingBox() {};
    CBoundingBox(const Vec2 & s)
        :size(s), halfSize(s.x / 2, s.y/2)
    {}
};

class CGravity : public Component
{
public:
    float gravity = 0;
    bool useGravity = true;
    CGravity() {}
    CGravity(float g) : gravity(g) {}
};

//need to change this
class CLifespan : public Component
{
public:
    int remaining = 0;
    int total = 0;

    CLifespan(){}
    CLifespan(int tot)
        :remaining(tot), total(tot)
    {}
};

class CInput : public Component
{
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool jump = false;

    CInput() {}
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeat = false; //what is this used for ?
    CAnimation(){};
    CAnimation(const Animation & a, bool r): animation{a}, repeat(r) {};

};

class CState : public Component
{
public:
    std::string state = "jumping";
    CState() {};
    CState(const std::string & s): state(s) {};
};

