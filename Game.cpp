#include "Game.h"
#include <iostream>
#include "Entity.h"
#include "EntityManager.h"
#include "Physics.h"
#include <random>
#include <ctime>
#include <string>
#include <fstream>

#include "Action.h"
#include "Assets.h"
#include "Animation.h"

int score = 0;
int maxEnemySpawned = 0;

struct WindowConfig
{
    std::string title;
    int width;
    int height;
    int frameLimit;
    int screenMode;
};

struct FontConfig
{
    std::string fontPath;
    int fontSize;
    int colorRed;
    int colorGreen;
    int colorBlue;
};

struct PlayerConfig
{
    int shapeRadius;
    int collisionRadius;
    float speed;
    int fillColorRed;
    int fillColorGreen;
    int fillColorBlue;
    int outlineColorRed;
    int outlineColorGreen;
    int outlineColorBlue;
    int outlineThickness;
    int shapeVertices;
};

struct EnemyConfig
{
    int shapeRadius;
    int collisionRadius;
    float minSpeed;
    float maxSpeed;
    int outlineColorRed;
    int outlineColorGreen;
    int outlineColorBlue;
    int outlineThickness;
    int minShapeVertices;
    int maxShapeVertices;
    int smallLifespan;
    int spawnInterval;
};

struct BulletConfig
{
    int shapeRadius;
    int collisionRadius;
    float speed;
    int fillColorRed;
    int fillColorGreen;
    int fillColorBlue;
    int outlineColorRed;
    int outlineColorGreen;
    int outlineColorBlue;
    int outlineThickness;
    int shapeVertices;
    int lifespan;
};

struct WindowConfig wConf;
struct FontConfig fConf;
struct PlayerConfig pConf;
struct EnemyConfig eConf;
struct BulletConfig bConf;


void loadConfigFile(const std::string & fileName)
{
    std::ifstream inputFile(fileName);
    std::string firstWordInLine;

    while(inputFile >> firstWordInLine)
    {
        if (firstWordInLine == "Window")
        {
            inputFile >> wConf.width >> wConf.height >> wConf.frameLimit >> wConf.screenMode;
        }
        else if (firstWordInLine == "Font" )
        {
            inputFile >> fConf.fontPath >> fConf.fontSize >> fConf.colorRed >> fConf.colorGreen>> fConf.colorBlue;
        }
        else if (firstWordInLine == "Player")
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
    std::cout << fConf.fontSize << "\n";

}


Game::Game()
{
    init();
}

// belong to scene class

void Game::registerAction(sf::Keyboard::Key keyCode, std::string actionType)
{
    m_actionMap.insert(std::pair<sf::Keyboard::Key, std::string>(keyCode, actionType));
}

const std::map<sf::Keyboard::Key, std::string> & Game::getActionMap() const
{
    return m_actionMap;
}

void Game::doAction(const Action & action)
{
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

// end scene class code

void Game::init()
{
    loadConfigFile("config.txt");
    //setupAssets();
    m_assets.addSound("test", "sound/DoubleBeep.wav");
    m_assets.addFont("test", fConf.fontPath );
    m_assets.addTexture("test", "sprite/walk.png");
    Animation playerAnimRun = {"test", m_assets.getTexture("test"), 6, 10};
    m_assets.addAnimation("test", playerAnimRun);

    //load the config file hered
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");

    //creating the window
    m_window.create(sf::VideoMode(wConf.width, wConf.height), "Game");
    m_window.setFramerateLimit(wConf.frameLimit);

    auto & m_font = m_assets.getFont("test");

    m_text.setFont(m_font);
    m_text.setCharacterSize(fConf.fontSize);
    m_text.setFillColor(sf::Color(fConf.colorRed, fConf.colorGreen, fConf.colorBlue));
    m_text.setStyle(sf::Text::Bold);

    spawnPlayer();
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::run()
{
    while(m_running)
    {

        m_entities.update();

        sUserInput();

        if(!m_paused)
        {
            sLifespan();
            sEnemySpawner();
            sMovement();
            sCollision();
            sAnimation();

        }

        sRender();

        m_currentFrame++;

    }
}

void Game::sRender()
{
    m_window.clear(sf::Color::Black);

    //rendering all game object here

    for (auto& e : m_entities.getEntities())
    {
        auto & entityPosX = e->getComponent<CTransform>().pos.x;
        auto & entityPosY = e->getComponent<CTransform>().pos.y;
        e->getComponent<CShape>().circle.setPosition(entityPosX, entityPosY);
        e->getComponent<CShape>().circle.setRotation(45.0f);
        //e->getComponent<CTransform>().angle += 1.0f;
        //e->getComponent<CShape>().circle.setRotation(e->getComponent<CTransform>().angle);
        if (e->hasComponent<CAnimation>())
        {
            e->getComponent<CAnimation>().animation.getSprite().setPosition(entityPosX, entityPosY);
            m_window.draw(e->getComponent<CAnimation>().animation.getSprite());
        }

        m_window.draw(e->getComponent<CShape>().circle);

    }


    m_text.setString("SCORE : " + std::to_string(score));
    m_window.draw(m_text);


    m_window.display();
}

void Game::sUserInput()
{


    sf::Event event;
    while(m_window.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            m_running = false;
            m_window.close();
        }

        if(event.type == sf::Event::KeyPressed || sf::Event::KeyReleased)
        {

        // action
            if(getActionMap().find(event.key.code) == getActionMap().end()) { continue; };
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
            doAction(Action(getActionMap().at(event.key.code), actionType));

        }


        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                m_assets.getSound("test").play();
                spawnBullet(m_player->getComponent<CTransform>().pos, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
        }
    }


}


void Game::sMovement()
{
    m_player->getComponent<CTransform>().velocity = {0,0};


    if (m_player->getComponent<CInput>().up &&
            m_player->getComponent<CTransform>().pos.y - m_player->getComponent<CShape>().circle.getRadius() > 0.0f)
    {
        m_player->getComponent<CTransform>().velocity.y = -1.0f;
    }

    if (m_player->getComponent<CInput>().down &&
            m_player->getComponent<CTransform>().pos.y + m_player->getComponent<CShape>().circle.getRadius() < m_window.getSize().y)
    {
        m_player->getComponent<CTransform>().velocity.y = 1.0f;
    }

    if (m_player->getComponent<CInput>().left &&
            m_player->getComponent<CTransform>().pos.x - m_player->getComponent<CShape>().circle.getRadius() > 0.0f)
    {
        m_player->getComponent<CTransform>().velocity.x = -1.0f;
    }

    if (m_player->getComponent<CInput>().right &&
            m_player->getComponent<CTransform>().pos.x + m_player->getComponent<CShape>().circle.getRadius() < m_window.getSize().x)
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

    for (auto &  b : m_entities.getEntities("Bullet"))
    {
        float angle = std::atan2(b->getComponent<CTransform>().velocity.x, b->getComponent<CTransform>().velocity.y);
        b->getComponent<CTransform>().pos.x += bConf.speed * std::sin(angle);
        b->getComponent<CTransform>().pos.y += bConf.speed * std::cos(angle);

        //std::cout << std::cos(angle) << "," << std::sin(angle) << "\n";

    }

    for (auto & sM : m_entities.getEntities("SmallEnemy"))
    {
        float sMSpeed =  eConf.minSpeed + (float) (rand()) / (float) (RAND_MAX / (eConf.maxSpeed - eConf.minSpeed));
        //std::cout << sMSpeed << "\n";

        sM->getComponent<CTransform>().pos.x += sMSpeed * sM->getComponent<CTransform>().velocity.x;
        sM->getComponent<CTransform>().pos.y += sMSpeed * sM->getComponent<CTransform>().velocity.y;
    }

}


void Game::spawnPlayer()
{
    auto newEntity = m_entities.addEntity("Player");
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
    newEntity->addComponent<CAnimation>();


    m_player = newEntity;
}

void Game::spawnBullet(const Vec2 & playerPos, const Vec2 & target)
{
    auto newBullet = m_entities.addEntity("Bullet");

    Vec2 dist = target - playerPos;

    newBullet->addComponent<CTransform>(playerPos, dist, Vec2(1.0f, 1.0f), 0.0f);
    newBullet->addComponent<CShape>(bConf.shapeRadius, bConf.shapeVertices,
                                                 sf::Color(bConf.fillColorRed, bConf.fillColorGreen, bConf.fillColorBlue),
                                                 sf::Color(bConf.outlineColorRed, bConf.outlineColorGreen, bConf.outlineColorBlue),
                                                 bConf.outlineThickness);
    newBullet->addComponent<CLifespan>(bConf.lifespan);
    //newBullet->addComponent<CCollision>(bConf.collisionRadius);
    const Vec2 size = {32.0f, 32.0f};
    newBullet->addComponent<CBoundingBox>( size );


}

void Game::spawnEnemy()
{
    auto newEntity = m_entities.addEntity("Enemy");

    float ex = rand() % (m_window.getSize().x - eConf.shapeRadius); //subtract its radius
    float ey = rand() % (m_window.getSize().y - eConf.shapeRadius);

    //size_t vert  = (size_t) rand() % (eConf.maxShapeVertices - eConf.minShapeVertices + 1) + eConf.minShapeVertices;
    size_t vert  = 4;
    //std::cout << vert << "\n";

    newEntity->addComponent<CTransform>(Vec2(ex, ey), Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f), 0.0f);
    newEntity->addComponent<CShape>(eConf.shapeRadius, vert,
                                                 sf::Color(255, 0, 0),
                                                 sf::Color(eConf.outlineColorRed, eConf.outlineColorGreen, eConf.outlineColorBlue, 0),
                                                 eConf.outlineThickness);
    //newEntity->cLifespan = std::make_shared<CLifespan>(300);
    //newEntity->addComponent<CCollision>(eConf.collisionRadius);
    const Vec2 size = {16.0f, 16.0f};
    newEntity->addComponent<CBoundingBox>( size );

    m_lastEnemySpawnTime = m_currentFrame;

}

void Game::spawnSmallEnemies(const std::shared_ptr<Entity> & parentEnemy, const Vec2 & vel, const float & angle)
{
    auto newSmallEnemy = m_entities.addEntity("SmallEnemy");

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

void Game::sLifespan()
{
    for (auto & b : m_entities.getEntities("Bullet"))
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

    for (auto & sM : m_entities.getEntities("SmallEnemy"))
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

    /**for (auto & e : m_entities.getEntities("Enemy"))
    {
        if (e->isActive())
        {
            e->cLifespan->remaining--;

            if (e->cLifespan->remaining <= 0)
            {
                e->destroy();
            }
        }
    }**/
}

void Game::sEnemySpawner()
{

    if (m_entities.getEntities("Enemy").size() <= maxEnemySpawned)
    {
        if ((m_currentFrame - m_lastEnemySpawnTime) % eConf.spawnInterval == 0)
        {
            spawnEnemy();
            //std::cout << m_entities.getEntities("Enemy").size() << "\n";
        }
    }
}

/**
void Game::sCollision()
{
    for(auto & e : m_entities.getEntities("Enemy"))
    {
        for(auto & b : m_entities.getEntities("Bullet"))
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

            for (auto & sM : m_entities.getEntities("SmallEnemy"))
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

            m_player->getComponent<CTransform>().pos = {m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f };
            score = 0;
            e->destroy();
        }
    }

    for (auto & sM : m_entities.getEntities("SmallEnemy"))
    {
        bool checkToP = m_physics.isCircleIntersect(sM->getComponent<CTransform>().pos.x, sM->getComponent<CTransform>().pos.y, sM->getComponent<CCollision>().radius,
                                                    m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y, m_player->getComponent<CCollision>().radius);
        //looping through the player to check collision here

        if(checkToP)
        {
            std::cout << "Player Damaged By Small Enemy. \n";
            m_player->getComponent<CTransform>().pos = {m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f };
            score = 0;
            sM->destroy();
        }
    }
}
**/

void Game::sCollision()
{
    for(auto & e : m_entities.getEntities("Enemy"))
    {
        m_physics.getOverlap(m_player, e);
    }
}

void Game::sAnimation()
{
    if (m_player->getComponent<CState>().state == "idle")
    {
        if (m_player->getComponent<CAnimation>().animation.getName() != "test")
        {
            m_player->addComponent<CAnimation>(m_assets.getAnimation("test"), true);
        }
    }


    for (auto & e : m_entities.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            e->getComponent<CAnimation>().animation.update();
        }
    }
}
