#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>
#include "AssetLoader.h"
#include <optional>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void onEnter() {}
    virtual void onExit() {}
    // SFML3: pollEvent returns std::optional<Event>
    virtual void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

#endif
