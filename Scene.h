#pragma once

#include "Action.h"
#include "EntityManager.h"
#include "GameEngine.h"
#include "Physics.h"
#include <memory>

class GameEngine;

typedef std::map<sf::Keyboard::Key, std::string> ActionMap;

namespace Camera
{
    enum Types
    {
        FollowX, FollowXY, Box, Zelda, Default
    };
}


class Scene
{
protected:
    GameEngine * m_game = nullptr;
    EntityManager m_entityManager;
    Physics m_physics;
    ActionMap m_actionMap;
    bool m_paused = false;
    bool m_hasEnded = false;
    size_t currentFrame = 0;

    bool m_moveCam = false;
    Camera::Types m_cameraType = Camera::Default;

    //virtual void onEnd();
    void setPaused(bool paused);

public:
    Scene();
    Scene(GameEngine * gameEngine);

    virtual void update(sf::Time deltaTime) = 0;
    virtual void sDoAction(const Action & action) = 0;
    virtual void sRender() = 0;
    void sCamera();

//    virtual void doAction(const Action & action) = 0; //dont need this for now
    //void simulate(const size_t frames);

    void registerAction(sf::Keyboard::Key inputKey, const std::string & actionName);
    const ActionMap & getActionMap() const;

    size_t windowWidth();
    size_t windowHeight();
//    size_t currentFrame();

    bool hasEnded() const; // for deleting scene
    void drawLine(const Vec2 & p1, const Vec2 & p2);
    sf::VertexArray createVertex(sf::PrimitiveType shapeType, size_t points);
};
