#include "Vec2.h"
#include <cmath>
#include <iostream>

Vec2::Vec2() {};

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin)
{
}

bool Vec2::operator == (const Vec2& rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
    return (x != rhs.x && y != rhs.y );
}

bool Vec2::operator < (const Vec2& rhs) const
{
    return (x < rhs.x && y < rhs.y );
}

bool Vec2::operator > (const Vec2& rhs) const
{
    return (x > rhs.x && y > rhs.y );
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float rhs) const
{
    return Vec2(x / rhs, y / rhs);
}


Vec2 Vec2::operator * (const float rhs) const
{
    return Vec2(x * rhs, y * rhs);
}

Vec2 Vec2::operator * (const Vec2 & rhs) const
{
    return Vec2(x * rhs.x, y * rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator /= (const float rhs)
{
    x /= rhs;
    y /= rhs;
}

void Vec2::operator *= (const float rhs)
{
    x *= rhs;
    y *= rhs;
}

float Vec2::length() const
{
    return std::sqrt(pow(x, 2.0) + pow(y, 2.0));
}

//still 20% more speed when moving diagonally
void Vec2::normalized()
{
    //std::cout << length() << "\n";
    x /= length();
    y /= length();
}

float Vec2::cross(const Vec2 & b)
{
    return x * b.y - y * b.x;
}

Vec2 Vec2::approach(Vec2 & goal, Vec2 & current, float dt)
{
    Vec2 diff = goal - current;

    if (diff.x > dt && diff.y > dt)
    {
        current.x += dt;
        current.y += dt;
        return current;
    }
    if (diff.x < -dt && diff.y < -dt)
    {
        current.x -= dt;
        current.y -= dt;
        return current;
    }

    return goal;
}
