#include "GameEngine.h"
#include "Action.h"
#include <fstream>

GameEngine::GameEngine(const std::string & path)
{
    init(path);
};


void GameEngine::init(const std::string & path)
{
    m_assets.loadFromFile(path);

    m_window.create(sf::VideoMode(800, 600), "Game Engine ?");
    m_window.setFramerateLimit(60);

    changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}

bool GameEngine::isRunning()
{
    return m_running && m_window.isOpen();
}

sf::RenderWindow & GameEngine::window()
{
    return m_window;
}

void GameEngine::run()
{
    while(isRunning())
    {
        update();
    }
}

void GameEngine::sUserInput()
{
    sf::Event event;

    while (m_window.pollEvent(event))
    {

        if (event.type == sf::Event::Closed)
        {
            quit();
        }


        if(event.type == sf::Event::KeyPressed || sf::Event::KeyReleased)
        {
            // action
            if(getActionMap().find(event.key.code) == getActionMap().end()) { continue; };
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
            doAction(Action(getActionMap().at(event.key.code), actionType));

        }

    }

}

void GameEngine::changeScene(const std::string & sceneName, std::shared_ptr<Scene> scene, bool currentScene = false )
{

}
