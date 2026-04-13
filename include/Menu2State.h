#ifndef MENU2STATE_H
#define MENU2STATE_H

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include "SaveManager.h"
#include <string>

class SceneManager;

class Menu2State : public Scene {
public:
    Menu2State(SceneManager* manager, const Save& save, int slotIndex, const std::string& slotPath);
    // convenience ctor when no specific slot/save (returns to menu2 with default save)
    explicit Menu2State(SceneManager* manager);

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

    sf::Texture bgTex;
    sf::Texture selTex, dressTex, rulesTex, backTex;
    sf::Texture level1Tex;
    sf::Texture level1LockedTex;
    std::unique_ptr<sf::Sprite> bg;
    sf::Font font;
    std::vector<sf::Text> buttons; // Level Select, Dressing, Rules, Back

    // optional sprites for buttons
    std::unique_ptr<sf::Sprite> selSprite;
    std::unique_ptr<sf::Sprite> level1Sprite;
    std::unique_ptr<sf::Sprite> level1LockedSprite;
    std::unique_ptr<sf::Sprite> dressSprite;
    std::unique_ptr<sf::Sprite> rulesBtnSprite;
    std::unique_ptr<sf::Sprite> backSprite;
    std::unique_ptr<sf::Sprite> rulesPageSprite; // Overlay sprite
    bool showingRules = false;
};

#endif
