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
        sf::View view = m_game->window().getView();
        sf::Vector2f windowCenter = {view.getSize().x / 2.0f, view.getSize().y / 2.0f};
        view.setCenter(windowCenter.x, windowCenter.y);
        m_game->window().setView(view);

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
        float boxSize = 50.f;
        float camSpeed = 300.f;

        float dx = std::abs(pPos.x - windowCenter.x);
        float dy = std::abs(pPos.y - windowCenter.y);
        bool isOutsideBox = dx > boxSize || dy > boxSize;
        bool isCenter = (pPos.x == windowCenter.x && pPos.y == windowCenter.y);

        float lerpX = m_physics.approach(pPos.x, windowCenter.x, m_game->deltaTime().asSeconds() * camSpeed);
        float lerpY = m_physics.approach(pPos.y, windowCenter.y, m_game->deltaTime().asSeconds() * camSpeed);

        if(isOutsideBox && m_moveCam == false)
        {
            m_moveCam = true;
        }

        if (m_moveCam)
        {
            view.setCenter(lerpX, lerpY);
            m_game->window().setView(view);
            if (isCenter)
            {
                m_moveCam = false;
            }
        }

        //render the box trap
        Vec2 topLeft = {windowCenter.x - boxSize, windowCenter.y - boxSize};
        Vec2 topRight = {windowCenter.x + boxSize, windowCenter.y - boxSize};
        Vec2 botLeft = {windowCenter.x - boxSize, windowCenter.y + boxSize};
        Vec2 botRight = {windowCenter.x + boxSize, windowCenter.y + boxSize};

        drawLine(topLeft, botLeft);
        drawLine(topLeft, topRight);
        drawLine(topRight, botRight);
        drawLine(botLeft, botRight);

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
