#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <vector>
#include <memory>
#include <iostream>

#include "Constants.h"
#include "Player.h"
#include "Decor.h"
#include "Road.h"
#include "Obstacle.h"
#include "Collectible.h"
#include "SideBuilding.h"
#include "GameOverUI.h"
#include "SaveManager.h"
#include "SceneManager.h"
#include "GameplayState.h"
#include "Menu1State.h"

struct Popup {
    sf::Sprite sprite;
    float timer;

    Popup(const sf::Texture& texture) : sprite(texture), timer(0.f) {}
};

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Pointes of No Return");
    window.setFramerateLimit(60);

    // Scene manager + start with main menu1 (save slots)
    SceneManager scenes;
    scenes.setScene(std::make_unique<Menu1State>(&scenes));

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (scenes.current()) scenes.current()->handleEvent(event, window);
        }

        if (scenes.current()) scenes.current()->update(dt);
        if (scenes.current()) scenes.current()->draw(window);
        window.display();
    }

    return 0;
}