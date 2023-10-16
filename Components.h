#pragma once

#include "SFML/Graphics.hpp"
#include "Vec2.h"

class Component
{
public:
    bool has = false;
};

class CTransform : public Component
{
public:
    Vec2 pos = {0.0, 0.0};
    Vec2 velocity = {0.0, 0.0};
    float angle = 0;

    CTransform() {}
    CTransform(const Vec2 & p, const Vec2 & vel, float a)
        :pos(p), velocity(vel), angle(a)
    {
    }
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

    CInput() {}
};
