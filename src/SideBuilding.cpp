#include "SideBuilding.h"

SideBuilding::SideBuilding(const sf::Texture& tex, bool rightSide, float startProgress)
    : sprite(tex), isRight(rightSide), progress(startProgress) {
    sf::FloatRect b = sprite.getLocalBounds();
    sprite.setOrigin({b.size.x / 2.f, b.size.y});
}

bool SideBuilding::update(float speed) {
    progress += speed * 0.3f;
    float s = 0.1f + (progress * 0.5f);
    float flip = isRight ? -1.f : 1.f;
    sprite.setScale({flip * s, s});

    float posY = (HORIZON_Y + 150.f) + (600.f * progress);
    float offsetX = 150.f + (progress * 500.f);
    float posX = isRight ? (500.f + offsetX) : (500.f - offsetX);

    sprite.setPosition({posX, posY});
    return progress > 1.5f || posY > WINDOW_HEIGHT + 600.f;
}