#pragma once

#include "Action.h"
#include "EntityManager.h"

#include <memory>

class GameEngine;

typedef std::map<int, std::string> ActionMap;

class Scene
{
protected:
    GameEngine * m_game = nullptr;
    EntityManager m_entities;
    ActionMap m_actionMap;
    bool m_paused = false;
    bool m_hasEnded = false;
    size_t currentFrame = 0;

    virtual void onEnd();
    void setPaused(bool paused);

public:
    Scene();
    Scene(GameEngine * gameEngine);

    virtual void update() = 0;
    virtual void sDoAction(const Action & action) = 0;
    virtual void sRender() = 0;

    virtual void doAction(const Action & action);
    void simulate(const size_t frames);
    void registerAction(int inputKey, const std::string & actionName);

    size_t width();
    size_t height();
    size_t currentFrame();

    bool hasEnded() const;
    const ActionMap & getActionMap() const;
    void drawLine(const Vec2 & p1, const Vec2 & p2);

};
