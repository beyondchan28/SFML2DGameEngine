#pragma once
#include <iostream>
#include <memory>
#include "SFML/Graphics.hpp"
#include "Vec2.h"

typedef sf::Sprite Sprite;

class Animation
{
    Sprite m_sprite;
    size_t m_frameCount;
    size_t m_gameFrame;
    size_t m_currentFrame;
    size_t m_speed;
    Vec2 m_size;
    std::string m_name;
    std::string m_texName;

public:
    Animation();
    Animation(const std::string & name, const sf::Texture & texture);
    Animation(const std::string & name, const sf::Texture & texture, size_t frameCount, size_t speed);
    void update();
    bool hasEnded();
    const std::string & getName();
    const Vec2 & getSize();
    const Sprite & getSprite();


};
