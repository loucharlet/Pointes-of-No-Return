#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

class Obstacle {
public:
    sf::Sprite sprite;
    int lane;
    float progress = 0.f;

    Obstacle(const sf::Texture& tex);
    bool update(float speed);
};

#endif