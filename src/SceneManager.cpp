#include "SceneManager.h"

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    if (currentScene) currentScene->onExit();
    currentScene = std::move(scene);
    if (currentScene) currentScene->onEnter();
}

Scene* SceneManager::current() {
    return currentScene.get();
}
