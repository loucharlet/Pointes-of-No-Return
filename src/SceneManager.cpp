#include "SceneManager.h"
#include "Constants.h"

#include "AppSettings.h"

SceneManager::SceneManager() {
    fadeRect.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
    fadeRect.setFillColor(sf::Color(0, 0, 0, 0));

    std::string musicPath = AssetLoader::findAudioPath("menu_theme.ogg");
    if (!musicPath.empty()) {
        if (menuMusic.openFromFile(musicPath)) {
            menuMusic.setLooping(true);
            menuMusic.setVolume(AppSettings::volume);
            if (AppSettings::musicOn) menuMusic.play();
        }
    }
}

void SceneManager::playMenuMusic() {
    if (menuMusic.getStatus() != sf::SoundSource::Status::Playing && AppSettings::musicOn) {
        menuMusic.play();
    }
}

void SceneManager::stopMenuMusic() {
    menuMusic.stop();
}

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    if (!currentScene) {
        currentScene = std::move(scene);
        currentScene->onEnter();
    } else {
        nextScene = std::move(scene);
        isFading = true;
        fadingToBlack = true;
    }
}

Scene* SceneManager::current() {
    return currentScene.get();
}

void SceneManager::update(float dt) {
    if (currentScene && !isFading) {
        currentScene->update(dt);
    } else if (isFading) {
        const float fadeSpeed = 500.f; // Ajuste ici pour la vitesse
        if (fadingToBlack) {
            fadeAlpha += fadeSpeed * dt;
            if (fadeAlpha >= 255.f) {
                fadeAlpha = 255.f;
                if (currentScene) currentScene->onExit();
                currentScene = std::move(nextScene);
                if (currentScene) currentScene->onEnter();
                fadingToBlack = false;
            }
        } else {
            fadeAlpha -= fadeSpeed * dt;
            if (fadeAlpha <= 0.f) {
                fadeAlpha = 0.f;
                isFading = false;
            }
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, (uint8_t)fadeAlpha));
        
        // On continue de mettre à jour la scène même pendant le fondu sortant
        if (currentScene && !fadingToBlack) {
            currentScene->update(dt);
        }
    }
}

void SceneManager::draw(sf::RenderWindow& window) {
    if (currentScene) {
        currentScene->draw(window);
    }
    if (fadeAlpha > 0.f) {
        window.draw(fadeRect);
    }
}
