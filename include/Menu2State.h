#ifndef MENU2STATE_H
#define MENU2STATE_H

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include "SaveManager.h"
#include <string>
#include "SettingsUI.h"

class SceneManager;

class Menu2State : public Scene {
public:
    Menu2State(SceneManager* manager, const Save& save, int slotIndex, const std::string& slotPath);
    explicit Menu2State(SceneManager* manager);

    void onEnter() override {}
    void onExit() override {}
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    void initCommon();

    SceneManager* scenes;
    Save saveData;
    int slotIndex;
    std::string slotPath;

    std::unique_ptr<sf::Sprite> bg;
    sf::Font font;
    std::vector<sf::Text> buttons;

    std::unique_ptr<sf::Sprite> selSprite;
    std::unique_ptr<sf::Sprite> level1Sprite;
    std::unique_ptr<sf::Sprite> level1LockedSprite;
    std::unique_ptr<sf::Sprite> dressSprite;
    std::unique_ptr<sf::Sprite> costumeSprite;
    std::unique_ptr<sf::Sprite> rulesBtnSprite;
    std::unique_ptr<sf::Sprite> backSprite;
    std::unique_ptr<sf::Sprite> rulesPageSprite;
    bool showingRules = false;

    // === Idle animation danseuse (ligne 0 uniquement, 6 frames stables) ===
    std::unique_ptr<sf::Sprite> idleSprite;
    std::vector<sf::IntRect> idleFrames;
    int   idleFrame = 0;
    float idleTimer = 0.f;
    static constexpr float IDLE_FRAME_DURATION = 0.29f;
    static constexpr float IDLE_SCALE          = 1.f;

    SettingsUI settingsUI;

    // === Souris ===
    struct MouseInstance {
        std::unique_ptr<sf::Sprite> sprite;
        sf::Vector2f pos;
        float scale;
    };
    std::vector<MouseInstance> mice;

    // Animation des souris
    int mouseFrame = 0;
    float mouseTimer = 0.f;
    static constexpr int MOUSE_FRAME_COUNT = 15;
    static constexpr int MOUSE_FRAME_WIDTH = 256;
    static constexpr int MOUSE_FRAME_HEIGHT = 256;
    static constexpr float MOUSE_FRAME_DURATION = 0.07f; 
};

#endif