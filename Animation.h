#pragma once
#include <iostream>
#include <memory>
#include "SFML/Graphics.hpp"
#include "Vec2.h"


class Animation
{
    sf::Sprite m_sprite;
    size_t m_frameCount = 1;
    size_t m_gameFrame = 0;
    size_t m_currentFrame = 0;
    size_t m_speed = 0;
    Vec2 m_size = {1, 1};
    std::string m_name = "none";


public:
    sf::Vector2f scale = {1.0f, 1.0f};
    Animation();
    Animation(const std::string & name, const sf::Texture & texture);
    Animation(const std::string & name, const sf::Texture & texture, size_t frameCount, size_t speed);
    void update();
    bool hasEnded() const;
    const std::string & getName();
    const Vec2 & getSize();
    sf::Sprite & getSprite();


};
