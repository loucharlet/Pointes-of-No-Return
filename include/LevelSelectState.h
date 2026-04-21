#ifndef LEVELSELECTSTATE_H
#define LEVELSELECTSTATE_H

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include "SaveManager.h"
#include "SettingsUI.h"

class SceneManager;

class LevelSelectState : public Scene {
public:
    explicit LevelSelectState(SceneManager* manager, const Save* saveOverride = nullptr, int slotIndex = -1, const std::string& slotPath = "");

    void onEnter() override {}
    void onExit() override {}
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(float dt) override {}
    void draw(sf::RenderWindow& window) override;

private:
    SceneManager* scenes;
    sf::Font font;
    struct LevelInfo { sf::FloatRect rect; bool unlocked; std::string name; };
    std::vector<LevelInfo> levels;
    Save save;
    int slotIndex = -1;
    std::string slotPath;

    // textures  logos
    std::vector<sf::Texture> imageUnlocked;
    std::vector<sf::Texture> imageLocked;
    sf::Texture backTex;
    std::unique_ptr<sf::Sprite> backBtn;
    sf::Texture bgTex;
    std::unique_ptr<sf::Sprite> bg;

    SettingsUI settingsUI;
};

std::unique_ptr<Scene> createLevelSelect(SceneManager* scenes, const Save* saveOverride = nullptr, int slotIndex = -1, const std::string& slotPath = "");

#endif
