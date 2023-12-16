#include "Scene_TopDown.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <iostream>
#include <fstream>
#include <deque>
#include <cmath>

std::deque<sf::VertexArray> walls; //for drawing
std::vector<std::vector<Vec2>> vertex; // for grouping vertex per walls/object
std::vector<Vec2> ungroupVertex; // for additional raycast
std::deque<sf::Vector2f> collidedVertex;

struct Spot
{
    float x, y;
    int f, g, h;

};

std::vector<std::vector<Spot>> grid(5);
std::vector<Spot> openSet;
std::vector<Spot> closedSet;

struct Spot startPoint;
struct Spot endPoint;

Scene_TopDown::Scene_TopDown(GameEngine * gameEngine, const std::string & levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(levelPath);
}

void Scene_TopDown::init(const std::string & levelPath)
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

void Scene_TopDown::loadLevel(const std::string & fileName)
{
    m_entityManager = EntityManager();

    m_cameraType = Camera::Box;

    loadConfigFile(fileName);

    spawnPlayer();
    setupWalls();

    AStarPathfinding();
}

void Scene_TopDown::loadConfigFile(const std::string & fileName)
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

void Scene_TopDown::spawnPlayer()
{
    auto player = m_entityManager.addEntity("Player");
    sf::Vector2u windowSize = m_game->window().getSize();
    player->addComponent<CTransform>(Vec2((float)windowSize.x / 2.0f, (float)windowSize.y / 2.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), 0.0f);
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

void Scene_TopDown::update(sf::Time deltaTime)
{
    m_entityManager.update();

    if(!m_paused)
    {
        sMovement(deltaTime);
        sCollision();

        sAnimation();

    }
    sRender();

    currentFrame++;
}

void Scene_TopDown::sRender()
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
//    auto & pPos = m_player->getComponent<CTransform>().pos;
//    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x );
//    sf::View view = m_game->window().getView();
//    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
//    m_game->window().setView(view);

    sf::Text levelText;
    levelText.setFont(m_game->assets().getFont("fontName"));
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setStyle(sf::Text::Bold);
    levelText.setPosition(50.0f,  200.0f);
    levelText.setString("LEVEL 2 TOPDOWN");
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

    renderWalls();
    raycastDetection();
//    renderSightPolygon();

    drawGrid(m_drawGrid);
}

void Scene_TopDown::raycastDetection()
{
    for (int w = 0; w < walls.size(); ++w)
    {
        for (auto & v : vertex)
        {
            const Vec2 & playerPos = m_player->getComponent<CTransform>().pos;
            Vec2 point1(playerPos.x, playerPos.y);
            Vec2 point2;
            Physics::Intersect intersection;
            sf::VertexArray rayCast = createRaycast(point1, point2, sf::Color::Black);
            Vec2 additionPoint2;
            sf::VertexArray AdditionRayCast = createRaycast(point1, additionPoint2, sf::Color::Red); //find a way to store this

            for (int i = 0; i < v.size(); ++i )
            {
                point2 = v[i];
                for (auto & l : vertex)
                {
                    for (int index = 0; index < l.size(); ++index)
                    {
                        if (index + 1 < l.size())
                        {
                            intersection = m_physics.lineIntersection(point1, point2, l[index], l[index+1]);
                        }
                        else
                        {
                            intersection = m_physics.lineIntersection(point1, point2, l[0], l[l.size()-1]);
                        }

                        if(intersection.result)
                        {
                            point2 = intersection.pos;
                        }
                    }
                }

                rayCast[1].position = sf::Vector2f(point2.x, point2.y);
                m_game->window().draw(rayCast);
            }

        }
    }

    for( auto & v : ungroupVertex)
    {
        const Vec2 & playerPos = m_player->getComponent<CTransform>().pos;
        Vec2 point1(playerPos.x, playerPos.y);
        Vec2 point2Left;
        Vec2 point2Right;
        float angle = std::atan2(v.y - point1.y, v.x - point1.x);

        point2Left.x = v.x + std::cos(angle - 0.0001f) * 800;
        point2Left.y = v.y + std::sin(angle - 0.0001f) * 800;

        point2Right.x = v.x + std::cos(angle + 0.0001f) * 800;
        point2Right.y = v.y + std::sin(angle + 0.0001f) * 800;

        Physics::Intersect intersectionLeft;
        Physics::Intersect intersectionRight;

        sf::VertexArray rayCastLeft = createRaycast(point1, point2Left, sf::Color::Red);
        sf::VertexArray rayCastRight = createRaycast(point1, point2Right, sf::Color::Yellow);
        for(auto & l : vertex)
        {
            for(int index = 0; index < l.size(); ++index)
            {
                if (index + 1 < l.size())
                {

                    intersectionLeft = m_physics.lineIntersection(point1, point2Left, l[index], l[index+1]);
                    intersectionRight = m_physics.lineIntersection(point1, point2Right, l[index], l[index+1]);
                }
                else
                {
                    intersectionLeft = m_physics.lineIntersection(point1, point2Left, l[0], l[l.size()-1]);
                    intersectionRight = m_physics.lineIntersection(point1, point2Right, l[0], l[l.size()-1]);
                }

                if(intersectionLeft.result)
                {
                    point2Left = intersectionLeft.pos;
                }
                if(intersectionRight.result)
                {
                    point2Right= intersectionRight.pos;
                }
            }


        }
            rayCastLeft[1].position = sf::Vector2f(point2Left.x, point2Left.y);
            m_game->window().draw(rayCastLeft);

            rayCastRight[1].position = sf::Vector2f(point2Right.x, point2Right.y);
            m_game->window().draw(rayCastRight);

    }

}

void Scene_TopDown::renderSightPolygon()
{
    sortSightPoint();

    if(!collidedVertex.empty())
    {
        sf::VertexArray polygonSight = createVertex(sf::TriangleFan, (int)collidedVertex.size()+1);
        const Vec2 & playerPos = m_player->getComponent<CTransform>().pos;
        polygonSight[0].position =  sf::Vector2f(playerPos.x, playerPos.y);
        for(int i = 0; i < collidedVertex.size(); ++i)
        {
            polygonSight[i+1].position = collidedVertex[i];
        }

        m_game->window().draw(polygonSight);
        collidedVertex.clear();
    }

//    if(!collidedVertex.empty())
//    {
//        sf::ConvexShape polygonSight;
//        polygonSight.setPointCount(collidedVertex.size() + 1);
//        const Vec2 & playerPos = m_player->getComponent<CTransform>().pos;
//        polygonSight.setPoint(0, sf::Vector2f(playerPos.x, playerPos.y));
//        for(int i = 0; i < collidedVertex.size(); ++i)
//        {
//            polygonSight.setPoint(i+1, collidedVertex[i]);
//        }
//
//        m_game->window().draw(polygonSight);
//        collidedVertex.clear();
//    }
}


void Scene_TopDown::sortSightPoint()
{
    if(!collidedVertex.empty())
    {
//        std::sort(collidedVertex.begin(), collidedVertex.end(), calcAngle());
//        std::sort(collidedVertex.begin(), collidedVertex.end(), [](sf::Vector2f a, sf::Vector2f b){ return a.y < b.y; });

    }
}

void Scene_TopDown::calculateRaycastAngle()
{
//    float angle = std::atan2(point2.y, point2.x);
//    point2.x = point1.x + std::cos(angle);
//    point2.y = point1.y + std::sin(angle);
}

void Scene_TopDown::renderWalls()
{
    for(int i = 0; i < walls.size() ; ++i)
    {
        m_game->window().draw(walls.at(i));
    }



}

void Scene_TopDown::setupWalls()
{
    //this needs to scan to all the vertex, not just itself
    sf::VertexArray screenRect = createVertex(sf::LineStrip, 5);
    static Vec2 screenCorner1(0.0f, 0.0f);
    static Vec2 screenCorner2((float)windowWidth(), 0.0f);
    static Vec2 screenCorner3((float)windowWidth(), (float)windowHeight());
    static Vec2 screenCorner4(0.0f, (float)windowHeight());
    std::vector<Vec2> screenVex =
    {
        screenCorner1, screenCorner2, screenCorner3, screenCorner4
    };
    vertex.push_back(screenVex);
    screenRect[0].position = sf::Vector2f(screenCorner1.x, screenCorner1.y);
    screenRect[1].position = sf::Vector2f(screenCorner2.x, screenCorner2.y);
    screenRect[2].position = sf::Vector2f(screenCorner3.x, screenCorner3.y);
    screenRect[3].position = sf::Vector2f(screenCorner4.x, screenCorner4.y);
    screenRect[4].position = sf::Vector2f(screenCorner1.x, screenCorner1.y);
    walls.push_back(screenRect);

    sf::VertexArray rect = createVertex(sf::LineStrip, 5);
    static Vec2 rectP1(100.f, 20.f);
    static Vec2 rectP2(150.f, 20.f);
    static Vec2 rectP3(150.f, 70.f);
    static Vec2 rectP4(100.f, 70.f);
    std::vector<Vec2> rectVex =
    {
        rectP1, rectP2, rectP3, rectP4
    };
    vertex.push_back(rectVex);
    rect[0].position = sf::Vector2f(rectP1.x, rectP1.y);
    rect[1].position = sf::Vector2f(rectP2.x, rectP2.y);
    rect[2].position = sf::Vector2f(rectP3.x, rectP3.y);
    rect[3].position = sf::Vector2f(rectP4.x, rectP4.y);
    rect[4].position = sf::Vector2f(rectP1.x, rectP1.y);
    walls.push_back(rect);


    sf::VertexArray triangle = createVertex(sf::Triangles, 3);
    static Vec2 triP1(400.f, 400.f);
    static Vec2 triP2(300.f, 350.f);
    static Vec2 triP3(300.f, 400.f);
    std::vector<Vec2> triVex =
    {
        triP1, triP2, triP3
    };
    vertex.push_back(triVex);
    triangle[0].position = sf::Vector2f(triP1.x, triP1.y);
    triangle[1].position = sf::Vector2f(triP2.x, triP2.y);
    triangle[2].position = sf::Vector2f(triP3.x, triP3.y);
    walls.push_back(triangle);
    for(auto arrVec : vertex)
    {
        for(auto vec : arrVec)
        {
            ungroupVertex.push_back(vec);
        }
    }

    std::cerr << ungroupVertex[5].x;

}



void Scene_TopDown::sDoAction(const Action & action)
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

void Scene_TopDown::sCollision()
{

    for(auto & t : m_entityManager.getEntities("Tile"))
    {
        const Vec2 & playerPos = m_player->getComponent<CTransform>().pos;
        const Vec2 & tilePos = t->getComponent<CTransform>().pos;

        bool isTileInRadius = m_physics.isPointInCircle(playerPos, (2.0f * 64.0f + 48.0f), tilePos)
                              ;
//        if(isTileInRadius)
//        {
//            tilesId.push(t->id());
//        }
    }
    for (auto & t : m_entityManager.getEntities("Tile"))
    {
        Vec2 overlapPos =  m_physics.getOverlap(m_player, t);
        bool isOverlap = m_physics.isOverlap(overlapPos);

        if(isOverlap)
        {
            Vec2 overlapDir;
            overlapDir = m_physics.getOverlapDirection(m_player, t);
            m_player->getComponent<CTransform>().pos += (overlapDir * overlapPos);
        }
    }
}


void Scene_TopDown::sMovement(sf::Time deltaTime)
{
    m_player->getComponent<CTransform>().velocity = {0, 0};

    if (m_player->getComponent<CInput>().left)
    {
        m_player->getComponent<CTransform>().velocity.x = -1.0f;
    }
    if (m_player->getComponent<CInput>().right)
    {
        m_player->getComponent<CTransform>().velocity.x = 1.0f;
    }

    if (m_player->getComponent<CInput>().up)
    {
        m_player->getComponent<CTransform>().velocity.y = -1.0f;
    }
    if (m_player->getComponent<CInput>().down)
    {
        m_player->getComponent<CTransform>().velocity.y = 1.0f;
    }

    //std::cout << m_player->cTransform->velocity.x << "," << m_player->cTransform->velocity.y  << "\n";
    if (m_player->getComponent<CTransform>().velocity.length() != 0.0f)
    {
        m_player->getComponent<CTransform>().velocity.normalized();
        //std::cout << m_player->cTransform->velocity.length() << "\n";
    }

    m_player->getComponent<CTransform>().pos += (m_player->getComponent<CTransform>().velocity * 400.0f * deltaTime.asSeconds()  ); // velocity times speed


//    std::cout << m_player->getComponent<CTransform>().velocity.x << " ";

}

void Scene_TopDown::sAnimation()
{
    if (m_player->getComponent<CState>().state == "walk_right")
    {
//        playerSpriteDir.x = 1.0f;
        if (m_player->getComponent<CAnimation>().animation.getName() != "walkPlayer")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("walkPlayer"), true);
        }
    }
    else if (m_player->getComponent<CState>().state == "walk_left")
    {
//        playerSpriteDir.x = -1.0f;
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

//    m_player->getComponent<CAnimation>().animation.getSprite().setScale(playerSpriteDir);


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

}

void Scene_TopDown::onEnd()
{
    m_hasEnded = true;
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_TopDown::settingUpStaticEntity(std::string entityType, std::string name, float gridX, float gridY)
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
Vec2 Scene_TopDown::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
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

void Scene_TopDown::drawCollision(std::shared_ptr<Entity> e, bool draw)
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

void Scene_TopDown::drawGrid(bool draw)
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



void Scene_TopDown::AStarPathfinding()
{

    for(int i = 0; i < grid.size(); ++i)
    {
        for(int j = 0; j < grid.size(); ++j)
        {
            struct Spot s;
            grid[i].push_back(s);

        }

    }
    std::cerr << grid[0].size() << "\n";
    std::cerr << grid.size() << "\n";

    startPoint = grid[0][0];
    endPoint = grid[grid.size() - 1][grid[0].size() - 1];

    openSet.push_back(startPoint);

    //looping
    if(!openSet.empty())
    {
        //keep going
    }
    else
    {
        //no solution
    }



}

