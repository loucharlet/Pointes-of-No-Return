#ifndef RULESSTATE_H
#define RULESSTATE_H

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include "SaveManager.h"
#include <string>
#include <sstream>
#include "SettingsUI.h"

class SceneManager;

class RulesState : public Scene {
public:
    RulesState(SceneManager* scenes, const Save& save, int slotIndex, const std::string& slotPath);

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
    bool showEnglish = false;

    sf::Texture bgTex;
    std::unique_ptr<sf::Sprite> bgSprite;
    sf::Texture backTex;
    std::unique_ptr<sf::Sprite> backBtn;

    sf::RectangleShape langBtn;
    sf::Texture rulesTex;
    std::unique_ptr<sf::Sprite> rulesSprite;
    sf::Texture quitTex;
    std::unique_ptr<sf::Sprite> quitSprite;

    SettingsUI settingsUI;
};

std::unique_ptr<Scene> createRules(SceneManager* scenes, const Save* save, int slotIndex, const std::string& slotPath);

#endif