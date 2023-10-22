#include "Animation.h"
#include <cmath>

Animation::Animation(){};

Animation::Animation(const std::string & name, const sf::Texture & texture)
    : Animation(name, texture, 1, 0)
{
}

Animation::Animation(const std::string & name, const sf::Texture & texture, size_t frameCount, size_t speed)
    : m_name(name)
    , m_sprite(texture)
    , m_frameCount(frameCount)
    , m_currentFrame(0)
    , m_speed(speed)
{
    m_size = Vec2((float)texture.getSize().x / frameCount, (float)texture.getSize().y);
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.y, m_size.y));
}

void Animation::update()
{
    m_currentFrame++;
}

bool Animation::hasEnded()
{
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

const Sprite & Animation::getSprite()
{
    return m_sprite;
}
