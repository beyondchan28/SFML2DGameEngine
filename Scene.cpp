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

size_t Scene::windowWidth()
{
    return m_game->window().getSize().x;
}

size_t Scene::windowHeight()
{
    return m_game->window().getSize().y;
}

const ActionMap & Scene::getActionMap() const
{
    return m_actionMap;
}

bool Scene::hasEnded() const
{
    return m_hasEnded;
}

void Scene::drawLine(const Vec2 & p1, const Vec2 & p2)
{
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(p1.x, p1.y)),
        sf::Vertex(sf::Vector2f(p2.x, p2.y))
    };
    m_game->window().draw(line, 2, sf::Lines);
}

sf::VertexArray Scene::createVertex(sf::PrimitiveType shapeType, size_t points)
{
    sf::VertexArray shape(shapeType, points);

    for(size_t i =0; i < points; ++i)
    {
        shape[i].color = sf::Color(255,255,255,125);
    }

    return shape;
}
