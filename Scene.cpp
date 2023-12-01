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

void Scene::sCamera()
{
    if(m_cameraType == Camera::Default)
    {
//        std::cerr << "work" << "\n";

    }
    else if(m_cameraType == Camera::FollowX)
    {
        //Centered view/camera if player going right enough
        auto & pPos = m_entityManager.getEntities("Player")[0]->getComponent<CTransform>().pos;
        float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x );
        sf::View view = m_game->window().getView();
        view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
        m_game->window().setView(view);
    }
    else if(m_cameraType == Camera::Box)
    {
        auto & pPos = m_entityManager.getEntities("Player")[0]->getComponent<CTransform>().pos;
        sf::View view = m_game->window().getView();
        const sf::Vector2f & windowCenter = view.getCenter();

        float halfSize = 50.f;

//        float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x );
//        float windowCenterY = std::max(m_game->window().getSize().y / 2.0f, pPos.y );

        Vec2 topLeft = {windowCenter.x - halfSize, windowCenter.y - halfSize};
        Vec2 topRight = {windowCenter.x + halfSize, windowCenter.y - halfSize};
        Vec2 botLeft = {windowCenter.x - halfSize, windowCenter.y + halfSize};
        Vec2 botRight = {windowCenter.x + halfSize, windowCenter.y + halfSize};

        drawLine(topLeft, botLeft);
        drawLine(topLeft, topRight);
        drawLine(topRight, botRight);
        drawLine(botLeft, botRight);

        if(pPos.x < topLeft.x && pPos.x < botLeft.x)
        {
            if(pPos.y > topLeft.y && pPos.y < botLeft.y)
            {
                view.setCenter(pPos.x, pPos.y);            }
        }
        else if(pPos.x > topRight.x && pPos.x > botRight.x)
        {
            if(pPos.y > topRight.y && pPos.y < botRight.y)
            {
                view.setCenter(pPos.x, pPos.y);

            }
        }
        else if(pPos.y < topLeft.y && pPos.y < topRight.y)
        {
            if(pPos.x > topLeft.x && pPos.x < topRight.x)
            {
                view.setCenter(pPos.x, pPos.y);
            }
        }

        //why the fuck is this wrong ?
        else if(pPos.y > botLeft.y && pPos.y > botRight.y)
        {
            if(pPos.x > botLeft.x && pPos.x < botRight.x)
            {
                view.setCenter(pPos.x, pPos.y);

            }
        }




        m_game->window().setView(view);

    }
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
