#ifndef DRESSINGSTATE_H
#define DRESSINGSTATE_H

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include "SaveManager.h"
#include <string>
#include "SettingsUI.h"

class SceneManager;

class DressingState : public Scene {
public:
    DressingState(SceneManager* scenes, const Save& save, int slotIndex, const std::string& slotPath);

    void onEnter() override {}
    void onExit() override {}
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(float dt) override {}
    void draw(sf::RenderWindow& window) override;

private:
    SceneManager* scenes;
    Save saveData;
    int slotIndex;
    std::string slotPath;

    sf::Font font;
    sf::Texture bgTex;
    sf::Texture backTex;
    sf::Texture invTex, invOpenTex;
    sf::Texture c1Tex, c2Tex, c3Tex, c4Tex;
    
    std::unique_ptr<sf::Sprite> dressingBg;
    std::unique_ptr<sf::Sprite> backBtn;
    std::unique_ptr<sf::Sprite> invBtn;
    std::unique_ptr<sf::Sprite> invOpenSprite;
    
    std::vector<sf::Sprite> collIcons;
    bool showingInventory = false;

    SettingsUI settingsUI;
};

// factory
std::unique_ptr<Scene> createDressing(SceneManager* scenes, const Save* save, int slotIndex, const std::string& slotPath);

#endif
