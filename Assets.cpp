#include "Assets.h"
#include "SFML/Audio.hpp"
#include <iostream>
#include <fstream>
#include <memory>

typedef std::vector<std::shared_ptr<sf::SoundBuffer>> SoundBufferVec;

SoundBufferVec m_soundBufferVec;

struct AssetsConf
{
    std::string name, path;
};

struct AssetsConf assetConf;


Assets::Assets() {}

void Assets::loadFromFile(const std::string & path)
{
    std::ifstream inputFile(path);
    std::string firstWordInLine;

     while(inputFile >> firstWordInLine)
    {
        if (firstWordInLine == "Texture")
        {
            inputFile >> assetConf.name >> assetConf.path;
            addTexture(assetConf.name, assetConf.path);
            std::cout << assetConf.name << "Load \n";
        }
        else if (firstWordInLine == "Sound" )
        {
            inputFile >> assetConf.name >> assetConf.path;
            addSound(assetConf.name, assetConf.path);
            std::cout << assetConf.name << "Load \n";
        }
        else if (firstWordInLine == "Font")
        {
            inputFile >> assetConf.name >> assetConf.path;
            addFont(assetConf.name, assetConf.path);
            std::cout << assetConf.name << "Load \n";

        }

    }
}

void Assets::addTexture(const std::string & name, const std::string & path)
{
    //auto texture = std::shared_ptr<sf::Texture>(new sf::Texture());
    sf::Texture texture;

    if(!texture.loadFromFile(path))
    {
        std::cout << "Load texture failed !" << "\n";
    }
    else
    {
        m_texturesMap.insert(std::pair<std::string, sf::Texture>(name, texture));
        std::cout << "Texture loaded" << "\n";
    }

}

void Assets::addAnimation(const std::string & name, const Animation & anim)
{
    m_animationMap.insert(std::pair<std::string, Animation>(name, anim));
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
        //auto sound = std::shared_ptr<sf::Sound>(new sf::Sound());
        sf::Sound sound;
        sound.setBuffer(*soundBuffer);
        m_soundsMap.insert(std::pair<std::string, sf::Sound>(name, sound));
        std::cout << "Sound loaded" << "\n";
    }
}

void Assets::addFont(const std::string & name, const std::string & path)
{
    //auto font = std::shared_ptr<sf::Font>(new sf::Font());
    sf::Font font;
    if(!font.loadFromFile(path))
    {
        std::cout << "Load font failed" << "\n";
    }
    else
    {
        m_fontsMap.insert(std::pair<std::string, sf::Font>(name, font));
        std::cout << "Font loaded" << "\n";
    }

}


sf::Texture & Assets::getTexture(const std::string & name)
{
    return m_texturesMap[name];
}

Animation & Assets::getAnimation(const std::string & name)
{
    return m_animationMap[name];
}

sf::Sound & Assets::getSound(const std::string & name)
{
    return m_soundsMap[name];
}

const sf::Font & Assets::getFont(const std::string & name)
{
    return m_fontsMap[name];
}
