#ifndef MENU1STATE_H
#define MENU1STATE_H

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include "SaveManager.h"

class SceneManager;

class Menu1State : public Scene {
public:
    explicit Menu1State(SceneManager* manager);

    void onEnter() override {}
    void onExit() override {}
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(float dt) override {}
    void draw(sf::RenderWindow& window) override;

private:
    SceneManager* scenes;
    sf::Texture bgTex;
    std::unique_ptr<sf::Sprite> bg;
    sf::Font font;
    std::vector<sf::Text> slotsText;
    std::vector<std::string> slotPaths;
    std::vector<sf::FloatRect> slotRects;
    sf::Texture slotBtnTex;
    std::unique_ptr<sf::Sprite> slotBtn; // Button_empty sprite prototype
    sf::Texture backTex;
    std::unique_ptr<sf::Sprite> backBtn;
    sf::Texture logoTex;
    std::unique_ptr<sf::Sprite> logo;
};

#endif
