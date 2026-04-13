#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include "Scene.h"

class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() { if (currentScene) currentScene->onExit(); }

    void setScene(std::unique_ptr<Scene> scene);
    Scene* current();

private:
    std::unique_ptr<Scene> currentScene;
};

#endif
