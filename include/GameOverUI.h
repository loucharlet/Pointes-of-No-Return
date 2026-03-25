
//inline coding: petite classe ds header

#ifndef GAMEOVERUI_H
#define GAMEOVERUI_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Constants.h"

class GameOverUI {
public:
    sf::Texture tex;
    std::unique_ptr<sf::Sprite> bg;
    sf::RectangleShape blackScreen;
    float alpha = 0.f;

    GameOverUI() {
        blackScreen.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
        blackScreen.setFillColor(sf::Color(0, 0, 0, 0));
    }

    bool load(const std::string& path) {
        if (!tex.loadFromFile(path)) return false;
        bg = std::make_unique<sf::Sprite>(tex);
        bg->setColor(sf::Color(255, 255, 255, 0));
        return true;
    }

    void updateFade(float dt, bool toBlack) {
        alpha += 150.f * dt; 
        if (alpha > 255.f) alpha = 255.f;
        uint8_t a = static_cast<uint8_t>(alpha);
        if (toBlack) blackScreen.setFillColor(sf::Color(0, 0, 0, a));
        else if (bg) bg->setColor(sf::Color(255, 255, 255, a));
    }

    void reset() {
        alpha = 0.f;
        blackScreen.setFillColor(sf::Color(0, 0, 0, 0));
        if (bg) bg->setColor(sf::Color(255, 255, 255, 0));
    }
};

#endif