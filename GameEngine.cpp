#include "GameEngine.h"
#include "Scene_Menu.h"
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
    //std::cout << m_currentScene << "\n";
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

const Assets & GameEngine::assets() const
{
    return m_assets;
}

Assets & GameEngine::assets()
{
    return m_assets;
}

const sf::Time & GameEngine::deltaTime()
{
    return m_deltaTime;
}

void GameEngine::run()
{
    while(isRunning())
    {
        update();
    }
}

void GameEngine::quit()
{
    m_window.close();
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
            if(currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; };
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }

    }

}

void GameEngine::changeScene(const std::string & sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (endCurrentScene == false)
    {
        if (m_sceneMap.count(sceneName) == 0)
        {
            m_sceneMap.insert(std::pair<std::string, std::shared_ptr<Scene>>(sceneName, scene));
        }

        m_currentScene = sceneName;
    }
}

void GameEngine::deleteScene()
{
    for(auto & [nameScene, scenePointer] : m_sceneMap)
    {
        if(scenePointer->hasEnded())
        {
            m_sceneMap.erase(nameScene);
        }
    }
}

void GameEngine::update()
{
    m_deltaTime = m_clock.restart();

    sUserInput();

    //loop all the systems here
    currentScene()->update(m_deltaTime);
    currentScene()->sCamera();


    window().display();

    deleteScene();
}
