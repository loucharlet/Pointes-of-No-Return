#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include "Scene.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager() { if (currentScene) currentScene->onExit(); }

    void setScene(std::unique_ptr<Scene> scene);
    Scene* current();

    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    std::unique_ptr<Scene> currentScene;
    std::unique_ptr<Scene> nextScene;
    float fadeAlpha = 0.f;
    bool isFading = false;
    bool fadingToBlack = true;
    sf::RectangleShape fadeRect;
};

#endif
