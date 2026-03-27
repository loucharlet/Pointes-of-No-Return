#ifndef ROAD_H
#define ROAD_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

class Road {
public:
    sf::VertexArray va;
    sf::Texture& texture;
    float offset = 0.f;

    Road(sf::Texture& tex);
    void update(float speed);
    void draw(sf::RenderWindow& window);
};

#endif