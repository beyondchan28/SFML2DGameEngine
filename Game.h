#pragma once
#include "SFML/Graphics.hpp"
#include "EntityManager.h"
#include "Physics.h"
#include <iostream>

#include "Action.h"
#include "Assets.h"


class Game
{
    sf::RenderWindow m_window;
    sf::Text m_text;

    Assets m_assets;

    EntityManager m_entities;
    Physics m_physics;

    std::shared_ptr<Entity> m_player;

    bool m_paused = false;
    bool m_running = true;

    int m_lastEnemySpawnTime = 0;
    int m_currentFrame = 0;

    void init();

    void setPaused(bool paused);

    void spawnPlayer();
    void spawnBullet(const Vec2 & playerPos, const Vec2 & target);

    void spawnSmallEnemies(const std::shared_ptr<Entity> & parentEnemy, const Vec2 & vel, const float & angle);


    void sMovement();
    void spawnEnemy();
    void sEnemySpawner();
    void sLifespan();

    void sCollision();
    bool circleIntersect(float x1, float y1, float r1, float x2, float y2, float r2);

    void sUserInput();
    void sRender();

    //below this should be on the scene class
    std::map<sf::Keyboard::Key, std::string> m_actionMap;

    void registerAction(sf::Keyboard::Key keyCode, std::string actionType);
    void doAction(const Action & action);

    const std::map<sf::Keyboard::Key, std::string> & getActionMap() const;

public:

    Game();
    void run();

};
