#pragma once

#include "Scene.h"
#include "EntityManager.h"
#include "Vec2.h"
#include <map>
#include <memory>


class Scene_Play : public Scene
{
    struct PlayerConfig
    {
        int shapeRadius, collisionRadius, fillColorRed, fillColorGreen,
            fillColorBlue, outlineColorRed, outlineColorGreen, outlineColorBlue,
            outlineThickness, shapeVertices;
        float speed;
    };

protected:
    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const Vec2 m_gridSize = {64, 64};
    sf::Text m_gridText;

    struct PlayerConfig m_playerConfig;

    void init(const std::string & levelPath);
    void loadLevel(const std::string & levelPath);
    void loadConfigFile(const std::string & filePath);
    void spawnPlayer();

    void update(sf::Time deltaTime);
    void sDoAction(const Action & action);
    void sAnimation();
    void sRender();
    void sMovement(sf::Time deltaTime);
    void sGravity();
    void sCollision();

    void onEnd();

    void settingUpStaticEntity(std::string entityType, std::string name, float gridX, float gridY);

    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

    void drawCollision(std::shared_ptr<Entity> e, bool draw);
    void drawGrid(bool draw);

public:
    Scene_Play(GameEngine * gameEngine, const std::string & levelPath);

};
