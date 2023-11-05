#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Animation.h"
#include <memory>

typedef std::map<std::string, sf::Texture> TextureMap;
typedef std::map<std::string, Animation> AnimationMap;
typedef std::map<std::string, sf::Sound> SoundMap;
typedef std::map<std::string, sf::Font> FontMap;



class Assets
{
    TextureMap m_texturesMap;
    AnimationMap m_animationMap;
    SoundMap m_soundsMap;
    FontMap m_fontsMap;

public:
    Assets();

    void loadFromFile(const std::string & path);

    void addTexture(const std::string & name, const std::string & path);
    void addAnimation(const std::string & name, const Animation & animation);
    void addSound(const std::string & name, const std::string & path);
    void addFont(const std::string & name, const std::string & path);

    sf::Texture & getTexture(const std::string & name);
    Animation & getAnimation(const std::string & name);
    sf::Sound & getSound(const std::string & name);
    const sf::Font & getFont(const std::string & name);
};
