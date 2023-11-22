#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <iostream>
#include <fstream>
#include <queue>

std::queue<size_t> tilesId;
std::vector<size_t> collidedTilesId;
sf::Vector2f playerSpriteDir = {1.0f, 1.0f};

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

    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Space, "JUMP");

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
        else if (firstWordInLine == "Tile")
        {
            std::string name, entityType;
            float gridX, gridY;

            entityType = firstWordInLine;

            inputFile >> name >> gridX >> gridY;
            settingUpStaticEntity(entityType, name, gridX, gridY);

        }
        else if (firstWordInLine == "Dec")
        {
            std::string name, entityType;
            float gridX, gridY;

            entityType = firstWordInLine;

            inputFile >> name >> gridX >> gridY;
            settingUpStaticEntity(entityType, name, gridX, gridY);

        }
    }
}

void Scene_Play::spawnPlayer()
{
    auto player = m_entityManager.addEntity("Player");
    player->addComponent<CTransform>(Vec2(100.0f, 50.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), 0.0f);
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
    player->addComponent<CGravity>(10.0f);
    player->addComponent<CRectangle>(size);

    m_player = player;
//    std::cout << m_entityManager.getEntities("Player")[0]->tag() << "\n";
}

void Scene_Play::update()
{
    m_entityManager.update();

    if(!m_paused)
    {
        sGravity();
        sMovement();
        sCollision();

        sAnimation();

    }

    sRender();

    currentFrame++;
}

void Scene_Play::sRender()
{
    //implement set the window to be darker if it paused
    if (!m_paused)
    {
        m_game->window().clear(sf::Color(100, 100, 255));
    }
    else
    {
        m_game->window().clear(sf::Color(50,50,150));
    }

    //Centered view/camera if player going right enough
    auto & pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x );
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);

    sf::Text levelText;
    levelText.setFont(m_game->assets().getFont("fontName"));
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setStyle(sf::Text::Bold);
    levelText.setPosition(50.0f,  200.0f);
    levelText.setString("LEVEL 1 SCENE");
    m_game->window().draw(levelText);


    for (auto& e : m_entityManager.getEntities())
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
            m_paused = !m_paused;
        }
        else if (action.name() == "TOGGLE_COLLISION")
        {
            m_drawCollision = !m_drawCollision;
        }
        else if (action.name() == "TOGGLE_TEXTURE")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (action.name() == "TOGGLE_GRID")
        {
            m_drawGrid = !m_drawGrid;
        }


        if(action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = true;
        }
        if(action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = true;
        }
        if(action.name() == "JUMP")
        {
            m_player->getComponent<CInput>().jump= true;
        }
        if(action.name() == "UP")
        {
            m_player->getComponent<CInput>().up = true;
        }
        if(action.name() == "DOWN")
        {
            m_player->getComponent<CInput>().down= true;
        }

    }

    if(action.type() == "END")
    {
        if(action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = false;
        }
        if(action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = false;
        }
        if(action.name() == "JUMP")
        {
            m_player->getComponent<CInput>().jump= false;
        }
        if(action.name() == "UP")
        {
            m_player->getComponent<CInput>().up = false;
        }
        if(action.name() == "DOWN")
        {
            m_player->getComponent<CInput>().down= false;
        }
    }
}

void Scene_Play::sCollision()
{
    for(auto & t : m_entityManager.getEntities("Tile"))
    {
        const Vec2 & playerPos = m_player->getComponent<CTransform>().pos;
        const Vec2 & tilePos = t->getComponent<CTransform>().pos;

        bool isTileInRadius = m_physics.isPointInCircle(playerPos, (2.0f * 64.0f + 48.0f), tilePos)
                              ;
        if(isTileInRadius)
        {
            tilesId.push(t->id());
        }
    }

    while (!tilesId.empty())
    {
        for (auto & t : m_entityManager.getEntities("Tile"))
        {
            if(tilesId.front() == t->id())
            {
                Vec2 overlapPos =  m_physics.getOverlap(m_player, t);
                bool isOverlap = m_physics.isOverlap(overlapPos);

                if(isOverlap)
                {
                    size_t tileId = tilesId.front();
                    collidedTilesId.push_back(tileId);

//                    std::cout << tileId << "\n";

                    Vec2 overlapDir;
                    if (t->id() == tileId)
                    {
                        overlapDir = m_physics.getOverlapDirection(m_player, t);
                        m_player->getComponent<CTransform>().pos += (overlapDir * overlapPos);
//                        std::cout << overlapDir.y << "\n";
                        if(overlapDir.y < 0.0f)
                        {
                            m_player->getComponent<CGravity>().useGravity = false;
                            m_player->getComponent<CTransform>().velocity.y = 0;
//                            std::cout << "overlapping" << "\n";
                        }
                    }
//                    std::cout << tileId << "\n";
                }
            }
        }
        tilesId.pop();
    }

    for (size_t tId : collidedTilesId)
    {
        for( auto & t : m_entityManager.getEntities("Tile"))
        {
            if(m_player->getComponent<CGravity>().useGravity == false)
            {

                if(t->id() == tId)
                {
                    Vec2 pPos = m_player->getComponent<CTransform>().pos;
                    Vec2 pHalfSize = m_player->getComponent<CBoundingBox>().halfSize;

                    Vec2 tPos = t->getComponent<CTransform>().pos;
                    Vec2 tHalfSize = t->getComponent<CBoundingBox>().halfSize;

                    float leftLimit = tPos.x - tHalfSize.x - (2 * pHalfSize.x);
                    float rightLimit = tPos.x + tHalfSize.x + (2 * pHalfSize.x);


                    //                    std::cout << pPos.x << "\n";
                    //                    std::cout << tPos.x  << "\n";
                    //                    std::cout << leftLimit << "\n";
                    //                    std::cout << rightLimit << "\n";

                    bool isNotOverlapTop = pPos.x < leftLimit ||  pPos.x > rightLimit;
                    if (isNotOverlapTop)
                    {
//                        std::cout << "not overlap" << "\n";
                        m_player->getComponent<CGravity>().useGravity = true;
                        //remove the tileId from the Vector

                    }
                }
            }
        }
    }



}

void Scene_Play::sGravity()
{
    for(auto & e: m_entityManager.getEntities())
    {
        if(e->hasComponent<CGravity>())
        {
            float & eVelY = e->getComponent<CTransform>().velocity.y;
            float & gravity = e->getComponent<CGravity>().gravity;

            if (m_player->getComponent<CGravity>().useGravity == true)
            {
                eVelY = 1.0f;
            }
            else if (m_player->getComponent<CGravity>().useGravity == false)
            {
                eVelY = 0.0f;
            }


//            eVelY += std::clamp(eVelY, -2.0f, gravity);
        }
    }
}

void Scene_Play::sMovement()
{
    m_player->getComponent<CTransform>().velocity.x = 0;

    if (m_player->getComponent<CInput>().left)
    {
        m_player->getComponent<CState>().state = "walk_left";
        m_player->getComponent<CTransform>().velocity.x = -1.0f;
    }
    else if (m_player->getComponent<CInput>().right)
    {
        m_player->getComponent<CState>().state = "walk_right";
        m_player->getComponent<CTransform>().velocity.x = 1.0f;
    }
    // need to find better condition than this
    else if (m_player->getComponent<CTransform>().velocity.y != 0.0f)
    {
        m_player->getComponent<CState>().state = "air";
    }
    else
    {
        m_player->getComponent<CState>().state = "idle";
    }


    if (m_player->getComponent<CInput>().up)
    {
        m_player->getComponent<CTransform>().velocity.y = -1.0f;
    }
    if (m_player->getComponent<CInput>().down)
    {
        m_player->getComponent<CTransform>().velocity.y = 1.0f;
    }

    // this is buggy
    if (m_player->getComponent<CInput>().jump)
    {
        m_player->getComponent<CTransform>().velocity.y -= m_playerConfig.speed * 2;
    }

    //std::cout << m_player->cTransform->velocity.x << "," << m_player->cTransform->velocity.y  << "\n";
//    if (m_player->getComponent<CTransform>().velocity.length() != 0.0f)
//    {
//        m_player->getComponent<CTransform>().velocity.normalized();
//        //std::cout << m_player->cTransform->velocity.length() << "\n";
//    }

    m_player->getComponent<CTransform>().pos += (m_player->getComponent<CTransform>().velocity * m_playerConfig.speed ); // velocity times speed


//    std::cout << m_player->getComponent<CTransform>().velocity.x << " ";

}

void Scene_Play::sAnimation()
{
    if (m_player->getComponent<CState>().state == "walk_right")
    {
        playerSpriteDir.x = 1.0f;
        if (m_player->getComponent<CAnimation>().animation.getName() != "walkPlayer")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("walkPlayer"), true);
        }
    }
    else if (m_player->getComponent<CState>().state == "walk_left")
    {
        playerSpriteDir.x = -1.0f;
        if (m_player->getComponent<CAnimation>().animation.getName() != "walkPlayer")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("walkPlayer"), true);
        }
    }
    else if (m_player->getComponent<CState>().state == "air")
    {
        if (m_player->getComponent<CAnimation>().animation.getName() != "airPlayer")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("airPlayer"), false);
        }
    }
    else if (m_player->getComponent<CState>().state == "idle")
    {
        if (m_player->getComponent<CAnimation>().animation.getName() != "idlePlayer")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("idlePlayer"), true);
        }
    }

    m_player->getComponent<CAnimation>().animation.getSprite().setScale(playerSpriteDir);


    for (auto & e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            if(e->getComponent<CAnimation>().repeat == true)
            {
                e->getComponent<CAnimation>().animation.update();
            }
        }
    }

    std::cerr << m_player->getComponent<CState>().state << "\n";
}

void Scene_Play::onEnd()
{
    m_hasEnded = true;
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::settingUpStaticEntity(std::string entityType, std::string name, float gridX, float gridY)
{
    Animation anim(name, m_game->assets().getTexture(name));

    auto staticEntity = m_entityManager.addEntity(entityType);
    staticEntity->addComponent<CAnimation>(anim, false);
    Vec2 posToPlaced = gridToMidPixel(gridX, gridY, staticEntity);
    staticEntity->addComponent<CTransform>(posToPlaced);

    if (entityType == "Tile")
    {
        Vec2 entitySize = staticEntity->getComponent<CAnimation>().animation.getSize();

        staticEntity->addComponent<CBoundingBox>(entitySize);
        staticEntity->addComponent<CRectangle>(entitySize);

    }
}

//its used to get position to be placed on the grid
Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    if (entity->hasComponent<CAnimation>())
    {
        auto textureSize = entity->getComponent<CAnimation>().animation.getSize();

        float halfWidthTexture = textureSize.x / 2;
        float halfHeightexture = textureSize.y / 2;

        float xPlacePos = halfWidthTexture + gridX * m_gridSize.x;
        float yPlacePos =(float)windowHeight() - (halfHeightexture + gridY * m_gridSize.y);

        return Vec2(xPlacePos, yPlacePos);
    }

}

void Scene_Play::drawCollision(std::shared_ptr<Entity> e, bool draw)
{
    if (draw)
    {
        auto & entityPosX = e->getComponent<CTransform>().pos.x;
        auto & entityPosY = e->getComponent<CTransform>().pos.y;

        e->getComponent<CRectangle>().rectangle.setPosition(entityPosX, entityPosY);
        m_game->window().draw(e->getComponent<CRectangle>().rectangle);

//        e->getComponent<CShape>().circle.setPosition(entityPosX, entityPosY);
//        e->getComponent<CShape>().circle.setRotation(45.0f);
        //e->getComponent<CTransform>().angle += 1.0f;
        //e->getComponent<CShape>().circle.setRotation(e->getComponent<CTransform>().angle);
//        m_game->window().draw(e->getComponent<CShape>().circle);
    }
}

void Scene_Play::drawGrid(bool draw)
{
    if (draw)
    {
        float leftX = m_game->window().getView().getCenter().x - windowWidth() / 2;
        float rightX = leftX + windowWidth() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for(float x = nextGridX; x < rightX; x += m_gridSize.x )
        {

            for(float y = 0; y < (float)windowHeight(); y += m_gridSize.y)
            {
                //vertical line
                drawLine(Vec2(x, 0), Vec2(x, (float)windowHeight()));


                //horizontal line
                drawLine(Vec2(leftX, (float)windowHeight() - y), Vec2(rightX, (float)windowHeight() - y));

                std::string xCell = std::to_string( (int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string( (int)y / (int)m_gridSize.y );
                m_gridText.setString("(" + xCell + "," + yCell+ ")" );
                m_gridText.setPosition(x + 3.0f, (float)windowHeight() - y - m_gridSize.y + 2.0f);
                m_game->window().draw(m_gridText);
            }
        }

    }
}
