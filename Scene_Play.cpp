#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <iostream>
#include <fstream>

Scene_Play::Scene_Play(GameEngine * gameEngine, const std::string & levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(levelPath);
}

void Scene_Play::init(const std::string & levelPath)
{
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");

    auto & assets = m_game->assets();

    m_gridText.setFont(assets.getFont("fontName"));
    m_gridText.setCharacterSize(8);
    m_gridText.setFillColor(sf::Color::White);
//    m_gridText.setStyle(sf::Text::Bold);

    loadLevel(levelPath);
}

void Scene_Play::loadLevel(const std::string & fileName)
{
    m_entityManager = EntityManager();

    loadConfigFile(fileName);

    spawnPlayer();
}

void Scene_Play::loadConfigFile(const std::string & fileName)
{
    std::ifstream inputFile(fileName);
    std::string firstWordInLine;

    while(inputFile >> firstWordInLine)
    {
        if (firstWordInLine == "Player")
        {
            inputFile >> m_playerConfig.shapeRadius >> m_playerConfig.collisionRadius >> m_playerConfig.speed >>
                        m_playerConfig.fillColorRed >> m_playerConfig.fillColorGreen >> m_playerConfig.fillColorBlue >>
                        m_playerConfig.outlineColorRed >> m_playerConfig.outlineColorGreen >> m_playerConfig.outlineColorBlue >>
                        m_playerConfig.outlineThickness >> m_playerConfig.shapeVertices;
        }
    }
}

void Scene_Play::spawnPlayer()
{
    auto player = m_entityManager.addEntity("Player");
    player->addComponent<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), 0.0f);
    player->addComponent<CShape>(m_playerConfig.shapeRadius, m_playerConfig.shapeVertices,
                                 sf::Color(m_playerConfig.fillColorRed, m_playerConfig.fillColorGreen, m_playerConfig.fillColorBlue, 0),
                                 sf::Color(m_playerConfig.outlineColorRed, m_playerConfig.outlineColorGreen, m_playerConfig.outlineColorBlue),
                                 m_playerConfig.outlineThickness);


    //newEntity->addComponent<CCollision>(pConf.collisionRadius);
    player->addComponent<CInput>();
    const Vec2 size = {16.0f, 16.0f};
    player->addComponent<CBoundingBox>( size );
    player->addComponent<CState>("idle");
    player->addComponent<CAnimation>();

    m_player = player;
//    std::cout << m_entityManager.getEntities("Player")[0]->tag() << "\n";
}

void Scene_Play::update()
{
    m_entityManager.update();

    if(!m_paused)
    {
        sAnimation();

    }

    sRender();

    currentFrame++;
}

void Scene_Play::sRender()
{
    sf::Text levelText;
    levelText.setFont(m_game->assets().getFont("fontName"));
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setStyle(sf::Text::Bold);
    levelText.setPosition(50.0f,  200.0f);
    levelText.setString("LEVEL 1 SCENE");
    m_game->window().draw(levelText);


    //this loop somehow not working
    for (auto& e : m_entityManager.getEntities("Player"))
    {
        if (e->hasComponent<CAnimation>())
        {
            drawCollision(e, m_drawCollision);

            if (m_drawTextures)
            {
                auto & entityPosX = e->getComponent<CTransform>().pos.x;
                auto & entityPosY = e->getComponent<CTransform>().pos.y;
                e->getComponent<CAnimation>().animation.getSprite().setPosition(entityPosX, entityPosY);
                m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
            }

        }

    }

    drawGrid(m_drawGrid);
}

void Scene_Play::sDoAction(const Action & action)
{
    if(action.type() == "START")
    {
        if(action.name() == "QUIT")
        {
            onEnd();
        }
        else if (action.name() == "PAUSE")
        {
            m_paused = (m_paused == true) ? false : true;
        }
        else if (action.name() == "TOGGLE_COLLISION")
        {
            m_drawCollision = (m_drawCollision == true) ? false : true;
        }
        else if (action.name() == "TOGGLE_TEXTURE")
        {
            m_drawTextures= (m_drawTextures == true) ? false : true;
        }
        else if (action.name() == "TOGGLE_GRID")
        {
            m_drawGrid = (m_drawGrid == true) ? false : true;
        }


        if(action.name() == "UP")
        {
            m_player->getComponent<CInput>().up= true;
        }
        if(action.name() == "DOWN")
        {
            m_player->getComponent<CInput>().down = true;
        }
        if(action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = true;
        }
        if(action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = true;
        }

    }

    if(action.type() == "END")
    {
        if(action.name() == "UP")
        {
            m_player->getComponent<CInput>().up= false;
        }
        if(action.name() == "DOWN")
        {
            m_player->getComponent<CInput>().down = false;
        }
        if(action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = false;
        }
        if(action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = false;
        }
    }
}

void Scene_Play::sAnimation()
{
    if (m_player->getComponent<CState>().state == "idle")
    {
        if (m_player->getComponent<CAnimation>().animation.getName() != "walk")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("walk"), true);
        }
    }


    for (auto & e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            e->getComponent<CAnimation>().animation.update();
        }
    }
}

void Scene_Play::onEnd()
{
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::drawCollision(std::shared_ptr<Entity> e, bool draw)
{
    if (draw)
    {
        auto & entityPosX = e->getComponent<CTransform>().pos.x;
        auto & entityPosY = e->getComponent<CTransform>().pos.y;

        e->getComponent<CShape>().circle.setPosition(entityPosX, entityPosY);
        e->getComponent<CShape>().circle.setRotation(45.0f);
        //e->getComponent<CTransform>().angle += 1.0f;
        //e->getComponent<CShape>().circle.setRotation(e->getComponent<CTransform>().angle);
        m_game->window().draw(e->getComponent<CShape>().circle);
    }
}

void Scene_Play::drawGrid(bool draw)
{
    if (draw)
    {
        const size_t & windowWidth = m_game->window().getSize().x;
        const size_t & windowHeight = m_game->window().getSize().y;
        for(float i = 64; i <= (float) windowWidth; i += 64 )
        {
            for(float j = 64; j <= (float) windowHeight; j += 64)
            {
                //vertical line
                sf::Vertex vertLine[] =
                {
                    sf::Vertex(sf::Vector2f(i-1.0f, 0)),
                    sf::Vertex(sf::Vector2f(i-1.0f, (float)windowHeight))
                };
                m_game->window().draw(vertLine, 2, sf::Lines);

                //horizontal line
                sf::Vertex horizLine[] =
                {
                    sf::Vertex(sf::Vector2f(0, j-1.0f)),
                    sf::Vertex(sf::Vector2f((float) windowWidth, j-1.0f))
                };
                m_game->window().draw(horizLine, 2, sf::Lines);

                m_gridText.setString("(" + std::to_string( (int) i ) + "," + std::to_string( (int)  j ) + ")" );
                m_gridText.setPosition(i - 64.0f, j - 64.0f);
                m_game->window().draw(m_gridText);
            }
        }
    }
}
