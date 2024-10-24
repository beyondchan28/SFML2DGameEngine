#include "Scene_GeometryShooter.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <random>
#include <ctime>
#include <fstream>
#include <iostream>

Scene_GeometryShooter::Scene_GeometryShooter(GameEngine * gameEngine, const std::string & levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(levelPath);
}

void Scene_GeometryShooter::init(const std::string & levelPath)
{
    loadConfigFile(levelPath);
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");

    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    //registerAction(sf::Mouse::Left, "FIRE");

    m_entityManager = EntityManager();
    auto & assets = m_game->assets();
    auto & m_font = assets.getFont("fontName");

    m_text.setFont(m_font);
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::White);
    m_text.setStyle(sf::Text::Bold);

    spawnPlayer();

}

void Scene_GeometryShooter::loadConfigFile(const std::string & fileName)
{
    std::ifstream inputFile(fileName);
    std::string firstWordInLine;

    while(inputFile >> firstWordInLine)
    {
        if (firstWordInLine == "Player")
        {
            inputFile >> pConf.shapeRadius >> pConf.collisionRadius >> pConf.speed >>
                        pConf.fillColorRed >> pConf.fillColorGreen >> pConf.fillColorBlue >>
                        pConf.outlineColorRed >> pConf.outlineColorGreen >> pConf.outlineColorBlue >>
                        pConf.outlineThickness >> pConf.shapeVertices;

        }
        else if (firstWordInLine == "Enemy")
        {
            inputFile >> eConf.shapeRadius >> eConf.collisionRadius >> eConf.minSpeed >> eConf.maxSpeed >>
                        eConf.outlineColorRed >> eConf.outlineColorGreen >> eConf.outlineColorBlue >>
                        eConf.outlineThickness >> eConf.minShapeVertices>> eConf.maxShapeVertices >>
                        eConf.smallLifespan >> eConf.spawnInterval;
        }
        else if (firstWordInLine == "Bullet")
        {
            inputFile >> bConf.shapeRadius >> bConf.collisionRadius >> bConf.speed >>
                        bConf.fillColorRed >> bConf.fillColorGreen >> bConf.fillColorBlue >>
                        bConf.outlineColorRed >> bConf.outlineColorGreen >> bConf.outlineColorBlue >>
                        bConf.outlineThickness >> bConf.shapeVertices >> bConf.lifespan;
        }

    }
}

void Scene_GeometryShooter::spawnPlayer()
{
    auto newEntity = m_entityManager.addEntity("Player");
    newEntity->addComponent<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), 0.0f);
    newEntity->addComponent<CShape>(pConf.shapeRadius, pConf.shapeVertices,
                                                 sf::Color(pConf.fillColorRed, pConf.fillColorGreen, pConf.fillColorBlue, 0),
                                                 sf::Color(pConf.outlineColorRed, pConf.outlineColorGreen, pConf.outlineColorBlue),
                                                 pConf.outlineThickness);


    //newEntity->addComponent<CCollision>(pConf.collisionRadius);
    newEntity->addComponent<CInput>();
    const Vec2 size = {16.0f, 16.0f};
    newEntity->addComponent<CBoundingBox>( size );
    newEntity->addComponent<CState>("idle");


    m_player = newEntity;
}

void Scene_GeometryShooter::spawnBullet(const Vec2 & playerPos, const Vec2 & target)
{
    auto newBullet = m_entityManager.addEntity("Bullet");

    Vec2 dist = target - playerPos;

    newBullet->addComponent<CTransform>(playerPos, dist, Vec2(1.0f, 1.0f), 0.0f);
    newBullet->addComponent<CShape>(bConf.shapeRadius, bConf.shapeVertices,
                                                 sf::Color(bConf.fillColorRed, bConf.fillColorGreen, bConf.fillColorBlue),
                                                 sf::Color(bConf.outlineColorRed, bConf.outlineColorGreen, bConf.outlineColorBlue),
                                                 bConf.outlineThickness);
    newBullet->addComponent<CLifespan>(bConf.lifespan);
    newBullet->addComponent<CCollision>(bConf.collisionRadius);
    const Vec2 size = {32.0f, 32.0f};
    newBullet->addComponent<CBoundingBox>( size );


}

void Scene_GeometryShooter::spawnEnemy()
{
    auto newEntity = m_entityManager.addEntity("Enemy");

    float ex = rand() % (m_game->window().getSize().x - eConf.shapeRadius); //subtract its radius
    float ey = rand() % (m_game->window().getSize().y - eConf.shapeRadius);

    size_t vert  = (size_t) rand() % (eConf.maxShapeVertices - eConf.minShapeVertices + 1) + eConf.minShapeVertices;

    newEntity->addComponent<CTransform>(Vec2(ex, ey), Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f), 0.0f);
    newEntity->addComponent<CShape>(eConf.shapeRadius, vert,
                                     sf::Color::Red,
                                     sf::Color(eConf.outlineColorRed, eConf.outlineColorGreen, eConf.outlineColorBlue),
                                     eConf.outlineThickness);
    newEntity->addComponent<CLifespan>(300);
    newEntity->addComponent<CCollision>(eConf.collisionRadius);

    m_lastEnemySpawnTime = currentFrame;

}

void Scene_GeometryShooter::spawnSmallEnemies(const std::shared_ptr<Entity> & parentEnemy, const Vec2 & vel, const float & angle)
{
    auto newSmallEnemy = m_entityManager.addEntity("SmallEnemy");

    //Vec2 posVec = {parentEnemy->cTransform->pos.normalized().y;

    newSmallEnemy->addComponent<CTransform>(parentEnemy->getComponent<CTransform>().pos, vel, Vec2(1.0f, 1.0f), angle);
    newSmallEnemy->addComponent<CShape>((float)(parentEnemy->getComponent<CShape>().circle.getRadius() / 3), parentEnemy->getComponent<CShape>().circle.getPointCount(),
                                                 sf::Color(255, 0, 0),
                                                 sf::Color(eConf.outlineColorRed, eConf.outlineColorGreen, eConf.outlineColorBlue),
                                                 eConf.outlineThickness);
    //newSmallEnemy->addComponent<CCollision>((parentEnemy->getComponent<CCollision>().radius / 3));
    newSmallEnemy->addComponent<CLifespan>(eConf.smallLifespan);
    const Vec2 size = {32.0f, 32.0f};
    newSmallEnemy->addComponent<CBoundingBox>( size );

}

void Scene_GeometryShooter::sDoAction(const Action & action)
{
    if(action.name() == "QUIT")
    {
        onEnd();
    }
    else if (action.name() == "PAUSE")
    {
        m_paused = !m_paused;
    }

    if(action.type() == "START")
    {
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
        //m_assets.getSound("test")->stop();
    }
}

void Scene_GeometryShooter::update(sf::Time deltaTime)
{
    m_entityManager.update();

    if(!m_paused)
    {
        sLifespan();
        sEnemySpawner();
        sMovement();
        sCollision();
    }

    sRender();

    currentFrame++;
}

void Scene_GeometryShooter::sRender()
{
    m_game->window().clear(sf::Color::Black);

    //rendering all game object here

    for (auto& e : m_entityManager.getEntities())
    {
        auto & entityPosX = e->getComponent<CTransform>().pos.x;
        auto & entityPosY = e->getComponent<CTransform>().pos.y;
        e->getComponent<CShape>().circle.setPosition(entityPosX, entityPosY);
        e->getComponent<CShape>().circle.setRotation(45.0f);
        e->getComponent<CTransform>().angle += 1.0f;
        e->getComponent<CShape>().circle.setRotation(e->getComponent<CTransform>().angle);

        m_game->window().draw(e->getComponent<CShape>().circle);

    }


    m_text.setString("SCORE : " + std::to_string(score));
    m_game->window().draw(m_text);

}

void Scene_GeometryShooter::sMovement()
{
    m_player->getComponent<CTransform>().velocity = {0,0};


    if (m_player->getComponent<CInput>().up &&
            m_player->getComponent<CTransform>().pos.y - m_player->getComponent<CShape>().circle.getRadius() > 0.0f)
    {
        m_player->getComponent<CTransform>().velocity.y = -1.0f;
    }

    if (m_player->getComponent<CInput>().down &&
            m_player->getComponent<CTransform>().pos.y + m_player->getComponent<CShape>().circle.getRadius() < m_game->window().getSize().y)
    {
        m_player->getComponent<CTransform>().velocity.y = 1.0f;
    }

    if (m_player->getComponent<CInput>().left &&
            m_player->getComponent<CTransform>().pos.x - m_player->getComponent<CShape>().circle.getRadius() > 0.0f)
    {
        m_player->getComponent<CTransform>().velocity.x = -1.0f;
    }

    if (m_player->getComponent<CInput>().right &&
            m_player->getComponent<CTransform>().pos.x + m_player->getComponent<CShape>().circle.getRadius() < m_game->window().getSize().x)
    {
        m_player->getComponent<CTransform>().velocity.x = 1.0f;
    }


    //std::cout << m_player->cTransform->velocity.x << "," << m_player->cTransform->velocity.y  << "\n";
    if (m_player->getComponent<CTransform>().velocity.length() != 0.0f)
    {
        m_player->getComponent<CTransform>().velocity.normalized();
        //std::cout << m_player->cTransform->velocity.length() << "\n";
    }

    m_player->getComponent<CTransform>().pos += (m_player->getComponent<CTransform>().velocity * pConf.speed ); // velocity times speed

    for (auto &  b : m_entityManager.getEntities("Bullet"))
    {
        float angle = std::atan2(b->getComponent<CTransform>().velocity.x, b->getComponent<CTransform>().velocity.y);
        b->getComponent<CTransform>().pos.x += bConf.speed * std::sin(angle);
        b->getComponent<CTransform>().pos.y += bConf.speed * std::cos(angle);

        //std::cout << std::cos(angle) << "," << std::sin(angle) << "\n";

    }

    for (auto & sM : m_entityManager.getEntities("SmallEnemy"))
    {
        float sMSpeed =  eConf.minSpeed + (float) (rand()) / (float) (RAND_MAX / (eConf.maxSpeed - eConf.minSpeed));
        //std::cout << sMSpeed << "\n";

        sM->getComponent<CTransform>().pos.x += sMSpeed * sM->getComponent<CTransform>().velocity.x;
        sM->getComponent<CTransform>().pos.y += sMSpeed * sM->getComponent<CTransform>().velocity.y;
    }

}

void Scene_GeometryShooter::sLifespan()
{
    for (auto & b : m_entityManager.getEntities("Bullet"))
    {
        if (b->isActive())
        {
            b->getComponent<CLifespan>().remaining--;
            //std::cout << b->cLifespan->remaining << "\n";
            if (b->getComponent<CLifespan>().remaining <= 0)
            {
                b->destroy();
            }
        }
    }

    for (auto & sM : m_entityManager.getEntities("SmallEnemy"))
    {
        if(sM->isActive())
        {
            sM->getComponent<CLifespan>().remaining--;

            if(sM->getComponent<CLifespan>().remaining <= 0)
            {
                sM->destroy();
            }
        }
    }
}

void Scene_GeometryShooter::sEnemySpawner()
{

    if (m_entityManager.getEntities("Enemy").size() <= maxEnemySpawned)
    {
        if ((currentFrame - m_lastEnemySpawnTime) % eConf.spawnInterval == 0)
        {
            spawnEnemy();
        }
    }
}

void Scene_GeometryShooter::sCollision()
{
    for(auto & e : m_entityManager.getEntities("Enemy"))
    {
        for(auto & b : m_entityManager.getEntities("Bullet"))
        {
            bool check = m_physics.isCircleIntersect(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y, e->getComponent<CCollision>().radius,
                                                    b->getComponent<CTransform>().pos.x, b->getComponent<CTransform>().pos.y, b->getComponent<CCollision>().radius);

            if (check)
            {
                //std::cerr << check << "\n";
                // Vec2 vel = {};
                float angle = 0.0f;
                for(int i = 0; i < e->getComponent<CShape>().circle.getPointCount(); ++i)
                {
                    angle += (((float) e->getComponent<CShape>().circle.getPointCount() -  2.0f) * 180.0f ) / (float) e->getComponent<CShape>().circle.getPointCount();
                    Vec2 vel = {std::sin(angle), std::cos(angle)};
                    spawnSmallEnemies(e, vel, angle);
                }
                e->destroy();
                b->destroy();
                angle = 0.0f;
                score++;
            }

            for (auto & sM : m_entityManager.getEntities("SmallEnemy"))
            {
                bool checkToB = m_physics.isCircleIntersect(sM->getComponent<CTransform>().pos.x, sM->getComponent<CTransform>().pos.y, sM->getComponent<CCollision>().radius,
                                                            b->getComponent<CTransform>().pos.x, b->getComponent<CTransform>().pos.y, b->getComponent<CCollision>().radius);
                //looping through the player to check collision here

                if(checkToB)
                {
                    std::cout << "Bullet Damaged Small Enemy. \n";
                    sM->destroy();
                }
            }

        }

        bool checkToP = m_physics.isCircleIntersect(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y, e->getComponent<CCollision>().radius,
                                                    m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y, m_player->getComponent<CCollision>().radius);
        //looping through the player to check collision here

        if(checkToP)
        {
            std::cout << "Player Damaged By Enemy. \n";

            m_player->getComponent<CTransform>().pos = {m_game->window().getSize().x / 2.0f, m_game->window().getSize().y / 2.0f };
            score = 0;
            e->destroy();
        }
    }

    for (auto & sM : m_entityManager.getEntities("SmallEnemy"))
    {
        bool checkToP = m_physics.isCircleIntersect(sM->getComponent<CTransform>().pos.x, sM->getComponent<CTransform>().pos.y, sM->getComponent<CCollision>().radius,
                                                    m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y, m_player->getComponent<CCollision>().radius);
        //looping through the player to check collision here

        if(checkToP)
        {
            std::cout << "Player Damaged By Small Enemy. \n";
            m_player->getComponent<CTransform>().pos = {m_game->window().getSize().x / 2.0f, m_game->window().getSize().y / 2.0f};
            score = 0;
            sM->destroy();
        }
    }
}


void Scene_GeometryShooter::onEnd()
{
    m_hasEnded = true;
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}
