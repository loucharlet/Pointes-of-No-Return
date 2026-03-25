#ifndef SIDEBUILDING_H
#define SIDEBUILDING_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

class SideBuilding {
public:
    sf::Sprite sprite;
    bool isRight;
    float progress = 0.f;

    SideBuilding(const sf::Texture& tex, bool rightSide, float startProgress = 0.f);
    bool update(float speed);
};

#endif