#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

class Collectible {
public:
    sf::Sprite sprite;
    int lane;
    float progress = 0.f;
    bool isCostomable = false;

    Collectible(const sf::Texture& tex, int l, bool costomable = false) : sprite(tex), lane(l), isCostomable(costomable) {
        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin({b.size.x / 2.f , b.size.y});
    }

    bool update(float speed) {
        progress += speed;

        float currentScale = progress * 1.7f;
        sprite.setScale({currentScale, currentScale});

        float startX = LANES_START_X[lane];
        float endX = LANES_END_X[lane] * 1.15f;
        float x = startX + (endX - startX) * progress;
        float y = (HORIZON_Y + 75.f) + (650.f * progress);

        sprite.setPosition({x, y});
        return progress > 1.0f;
    }
};

#endif