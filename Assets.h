#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <memory>

typedef std::map<std::string, std::shared_ptr<sf::Texture>> TextureMap;
//typedef std::map<std::string, Animation> AnimationMap;
typedef std::map<std::string, std::shared_ptr<sf::Sound>> SoundMap;
typedef std::map<std::string, std::shared_ptr<sf::Font>> FontMap;



class Assets
{
    TextureMap m_texturesMap;
    SoundMap m_soundsMap;
    FontMap m_fontsMap;

public:
    Assets();

    void addTexture(const std::string & name, const std::string & path);
    void addSound(const std::string & name, const std::string & path);
    void addFont(const std::string & name, const std::string & path);

    std::shared_ptr<sf::Texture> & getTexture(std::string name);
    std::shared_ptr<sf::Sound> & getSound(const std::string & name);
    std::shared_ptr<sf::Font> & getFont(std::string name);
};
