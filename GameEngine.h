#pragma once

#include "Assets.h"
#include "Scene.h"

#include <memory>

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
protected:
    sf::RenderWindow m_window;
    Assets m_assets;
    std::string m_currentScene;
    SceneMap m_sceneMap;
    size_t simulationSpeed;
    bool m_running = true;

    void init(const std::string & path);
    void update(); // the scene gameplay systems inside game loop

    void sUserInput();

    std::shared_ptr<Scene> currentScene();

public:
    GameEngine(const std::string & path); //path for loading config file to loads all the assets
    void changeScene(const std::string & sceneName, std::shared_ptr<Scene> scene, bool currentScene = false);

    void quit();
    void run();

    sf::RenderWindow & window();
    const Assets & assets() const;
    bool isRunning();
};
