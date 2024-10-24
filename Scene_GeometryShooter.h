#pragma once

#include "Scene.h"
#include "EntityManager.h"
#include "Vec2.h"
#include <map>
#include <memory>


class Scene_GeometryShooter: public Scene
{
    struct PlayerConfig
    {
        int shapeRadius,
            collisionRadius,
            fillColorRed,
            fillColorGreen,
            fillColorBlue,
            outlineColorRed,
            outlineColorGreen,
            outlineColorBlue,
            outlineThickness,
            shapeVertices;
        float speed;
    };

    struct EnemyConfig
    {
        int shapeRadius,
            collisionRadius,
            outlineColorRed,
            outlineColorGreen,
            outlineColorBlue,
            outlineThickness,
            minShapeVertices,
            maxShapeVertices,
            smallLifespan,
            spawnInterval;
        float minSpeed, maxSpeed;
    };

    struct BulletConfig
    {
        int shapeRadius,
            collisionRadius,
            fillColorRed,
            fillColorGreen,
            fillColorBlue,
            outlineColorRed,
            outlineColorGreen,
            outlineColorBlue,
            outlineThickness,
            shapeVertices,
            lifespan;
        float speed;
    };

protected:
    int score = 0;
    int maxEnemySpawned = 0;

    struct PlayerConfig pConf;
    struct EnemyConfig eConf;
    struct BulletConfig bConf;

    sf::Text m_text;
    std::string m_levelPath;
    std::shared_ptr<Entity> m_player;

    int m_lastEnemySpawnTime = 0;

    void init(const std::string & levelPath);
    void loadConfigFile(const std::string & fileName);
    void spawnPlayer();
    void spawnBullet(const Vec2 & playerPos, const Vec2 & target);
    void spawnSmallEnemies(const std::shared_ptr<Entity> & parentEnemy, const Vec2 & vel, const float & angle);
    void spawnEnemy();

    void update(sf::Time deltaTime);
    void sRender();
    void sDoAction(const Action & action);
    void sMovement();
    void sEnemySpawner();
    void sLifespan();
    void sCollision();
    void sAnimation();
    void onEnd();

public:
    Scene_GeometryShooter(GameEngine * gameEngine, const std::string & levelPath);

};
