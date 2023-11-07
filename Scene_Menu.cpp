#include "Scene_Menu.h"
#include "Scene.h"
#include "Scene_Play.h"

Scene_Menu::Scene_Menu(GameEngine * gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    m_menuStrings.push_back("LEVEL 1");
    m_menuStrings.push_back("LEVEL 2");

    //configurationFilePath
    m_levelPaths.push_back("level1Conf.txt");
    m_levelPaths.push_back("LEVEL2");

    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Enter, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_title = "MAIN MENU";

    auto & assets = m_game->assets();

    m_menuText.setFont(assets.getFont("fontName"));
    m_menuText.setCharacterSize(24);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setStyle(sf::Text::Bold);
    m_menuText.setString(m_title);
}

void Scene_Menu::update()
{
    sRender();

    currentFrame++;
}

void Scene_Menu::sRender()
{
    std::string levelName = m_menuStrings[m_selectedMenuIndex];
    m_game->window().draw(m_menuText);

    float posX = 50.0f;
    float posY = 50.0f;

    sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
    rectangle.setFillColor(sf::Color::Cyan);
    rectangle.setPosition(posX, posY * (float) (m_selectedMenuIndex + 1) );
    m_game->window().draw(rectangle);

    for (int i = 0; i < m_menuStrings.size(); ++i )
    {
        sf::Text levelText;
        levelText.setFont(m_game->assets().getFont("fontName"));
        levelText.setCharacterSize(24);
        levelText.setFillColor(sf::Color::White);
        levelText.setStyle(sf::Text::Bold);
        levelText.setPosition(posX, posY * (float)(i + 1) );
        levelText.setString(m_menuStrings[i]);
        m_game->window().draw(levelText);

    }

}


void Scene_Menu::sDoAction(const Action & action)
{
    if(action.type() == "START")
    {
        if(action.name() == "UP")
        {
            if(m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1;  }
        }
        else if(action.name() == "DOWN")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        else if(action.name() == "PLAY")
        {
            m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
        }
        else if(action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

//void Scene_Menu::doAction(const Action & action)
//{
//
//}

void Scene_Menu::onEnd()
{
    m_game->quit();
}

