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
#include "AssetLoader.h"

struct Popup {
    sf::Sprite sprite;
    float timer;

    Popup(const sf::Texture& texture) : sprite(texture), timer(0.f) {}
};

sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {
    float windowRatio = (float)windowWidth / (float)windowHeight;
    float viewRatio = view.getSize().x / view.getSize().y;
    float sizeX = 1.f;
    float sizeY = 1.f;
    float posX = 0.f;
    float posY = 0.f;

    if (windowRatio > viewRatio) {
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    } else {
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport({ {posX, posY}, {sizeX, sizeY} });
    return view;
}

int main() {
    AssetLoader::preloadAll();
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Pointes of No Return", sf::Style::Default | sf::Style::Resize);
    window.setFramerateLimit(60);

    sf::View view;
    view.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
    view.setCenter({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    window.setView(getLetterboxView(view, (int)WINDOW_WIDTH, (int)WINDOW_HEIGHT));

    // Scene manager + start with main menu1 (save slots)
    SceneManager scenes;
    scenes.setScene(std::make_unique<Menu1State>(&scenes));

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                window.setView(getLetterboxView(view, (int)resized->size.x, (int)resized->size.y));
            }
            if (scenes.current()) scenes.current()->handleEvent(event, window);
        }

        scenes.update(dt);
        scenes.draw(window);
        window.display();
    }

    return 0;
}