#include "Animation.h"
#include "SFML/Graphics.hpp"
#include <cmath>
#include <iostream>

Animation::Animation(){};

Animation::Animation(const std::string & name, const sf::Texture & texture, bool animate )
    : Animation(name, texture, 1, 0, animate)
{
}

Animation::Animation(const std::string & name, const sf::Texture & texture, size_t frameCount, size_t speed, bool animate)
    : m_name(name)
    , m_sprite(texture)
    , m_frameCount(frameCount)
    , m_currentFrame(0)
    , m_speed(speed)
    , m_animate(animate)
{
    m_size = Vec2((float)texture.getSize().x / frameCount, (float)texture.getSize().y);
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

void Animation::update()
{
    if (m_animate)
    {
        m_currentFrame++;

        size_t animFrame = (m_currentFrame / m_speed) % m_frameCount;
        sf::IntRect rectangle = {animFrame * m_size.x, 0, m_size.x, m_size.y};
        m_sprite.setTextureRect(rectangle);
        //std::cout << m_sprite.getTextureRect().left << "\n";
        //std::cout << m_currentFrame << "\n";
    }

}

bool Animation::hasEnded() const
{
    //need to implement this later
    return false;
}


const std::string & Animation::getName()
{
    return m_name;
}

const Vec2 & Animation::getSize()
{
    return m_size;
}

sf::Sprite & Animation::getSprite()
{
    return m_sprite;
}
