#include "Assets.h"
#include "SFML/Audio.hpp"
#include <iostream>
#include <memory>

typedef std::vector<std::shared_ptr<sf::SoundBuffer>> SoundBufferVec;

SoundBufferVec m_soundBufferVec;

Assets::Assets() {}

void Assets::addTexture(const std::string & name, const std::string & path)
{
    auto texture = std::shared_ptr<sf::Texture>(new sf::Texture());

    if(!texture->loadFromFile(path))
    {
        std::cout << "Load texture failed !" << "\n";
    }
    else
    {
        m_texturesMap.insert(std::pair<std::string, std::shared_ptr<sf::Texture>>(name, texture));
        std::cout << "Texture loaded" << "\n";
    }

}

void Assets::addAnimation(const std::string & name, const Animation & anim)
{

}

void Assets::addSound(const std::string & name, const std::string & path)
{
    auto soundBuffer = std::shared_ptr<sf::SoundBuffer>(new sf::SoundBuffer());

    if(!soundBuffer->loadFromFile(path))
    {
        std::cout << "Load sound failed !" << "\n";
    }
    else
    {
        m_soundBufferVec.push_back(soundBuffer);
        auto sound = std::shared_ptr<sf::Sound>(new sf::Sound());
        sound->setBuffer(*soundBuffer);
        m_soundsMap.insert(std::pair<std::string, std::shared_ptr<sf::Sound>>(name, sound));
        std::cout << "Sound loaded" << "\n";
    }
}

void Assets::addFont(const std::string & name, const std::string & path)
{
    auto font = std::shared_ptr<sf::Font>(new sf::Font());
    if(!font->loadFromFile(path))
    {
        std::cout << "Load font failed" << "\n";
    }
    else
    {
        m_fontsMap.insert(std::pair<std::string, std::shared_ptr<sf::Font>>(name, font));
        std::cout << "Font loaded" << "\n";
    }

}


std::shared_ptr<sf::Texture> & Assets::getTexture(const std::string & name)
{
    return m_texturesMap[name];
}

Animation & Assets::getAnimation(const std::string & name)
{
    return m_animationMap[name];
}

std::shared_ptr<sf::Sound> & Assets::getSound(const std::string & name)
{
    return m_soundsMap[name];
}

std::shared_ptr<sf::Font> & Assets::getFont(const std::string & name)
{
    return m_fontsMap[name];
}
