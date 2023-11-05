#include "Scene.h"
#include "GameEngine.h"

Scene::Scene() {}

Scene::Scene(GameEngine * gameEngine)
    : m_game(gameEngine)
{
}



//void Scene::simulate(const size_t frames)
//{
//
//}

void Scene::registerAction(sf::Keyboard::Key inputKey, const std::string & actionName)
{
    m_actionMap.insert(std::pair<sf::Keyboard::Key, std::string>(inputKey, actionName));
}

//size_t Scene::width()
//{
//
//}

//size_t Scene::height()
//{
//
//}

const ActionMap & Scene::getActionMap() const
{
    return m_actionMap;
}

//bool Scene::hasEnded() const
//{
//    return m_hasEnded;
//}

