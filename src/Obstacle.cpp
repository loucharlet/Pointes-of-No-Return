#include "Obstacle.h"

Obstacle::Obstacle(const sf::Texture& tex) : sprite(tex), lane(rand() % 3) {}

bool Obstacle::update(float speed) {
    progress += speed * 1.2f;
    float s = 0.01f + (progress * 0.09f);
    sprite.setScale({s, s});
    sf::FloatRect b = sprite.getLocalBounds();
    sprite.setOrigin({b.size.x / 2.f, b.size.y});

    float posX = LANES_START_X[lane] + (LANES_END_X[lane] - LANES_START_X[lane]) * progress;
    float posY = (HORIZON_Y + 100.f) + (650.f * progress);

    sprite.setPosition({posX, posY});
    return progress > 1.1f;
}
