#ifndef COSTUMESTATE_H
#define COSTUMESTATE_H

#include "Scene.h"
#include "SaveManager.h"
#include "SettingsUI.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

class SceneManager;

class CostumeState : public Scene {
public:
    CostumeState(SceneManager* scenes, const Save& save, int slotIndex, const std::string& slotPath);

    void onEnter() override {}
    void onExit() override {}
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(float dt) override {}
    void draw(sf::RenderWindow& window) override;

private:
    SceneManager* scenes = nullptr;
    Save saveData;
    int slotIndex = -1;
    std::string slotPath;

    sf::Font font;
    sf::Texture bgTex;
    std::unique_ptr<sf::Sprite> bg;

    sf::Texture backTex;
    std::unique_ptr<sf::Sprite> backBtn;

    std::unique_ptr<sf::Sprite> costuventoryBtn;

    bool showingCostumes = false;
    sf::RectangleShape overlay; // Dim background when modal is open
    sf::RectangleShape panel;
    
    struct CostumeUI {
        std::string id;
        std::unique_ptr<sf::Sprite> modalSprite;
        std::unique_ptr<sf::Sprite> btnSprite;
        int requiredCostomables;
        bool isUnlocked;
    };
    std::vector<CostumeUI> costumes;

    void updateCostumesUI();
    void equip(const std::string& id);

    SettingsUI settingsUI;
};

std::unique_ptr<Scene> createCostume(SceneManager* scenes, const Save* save, int slotIndex, const std::string& slotPath);

#endif

