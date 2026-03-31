#include "Player.h"

Player::Player(const sf::Texture& tex) : sprite(tex) {
    runFrames = {{{0,3},{170,210}}, {{185,3},{170,210}}, {{380,0},{170,210}}, {{560,0},{170,210}}, {{730,0},{170,210}}, {{950,0},{170,210}}};
    jumpFrames = {{{0,590},{170,250}}, {{170,590},{170,250}}, {{340,590},{170,250}}, {{550,590},{170,250}}, {{710,590},{170,250}}, {{890,590},{170,250}}};
    moveFrames = {{{0,410},{170,175}}, {{190,410},{170,175}}, {{360,410},{170,178}}, {{560,410},{170,177}}, {{730,410},{170,175}}, {{950,410},{170,175}}};
    dieFrames = {{{0,235},{170,175}}, {{179,230},{170,175}}, {{380,230},{170,175}}, {{560,230},{170,175}}, {{740,230},{170,175}}, {{935,230},{170,175}}};

    updateSpriteRect(runFrames[0]);
}

void Player::updateSpriteRect(sf::IntRect rect) {
    sprite.setTextureRect(rect);
    sprite.setOrigin({(float)rect.size.x / 2.f, (float)rect.size.y});
}

void Player::jump() {
    if (state == PlayerState::RUN) {
        state = PlayerState::JUMP;
        vY = -14.f;
        jumpDelay = 0.2f;
        frameNum = 0;
    }
}

void Player::changeLane(int newLane) {
    if (state != PlayerState::DIE) {
        if (newLane < lane) lastDir = -1.0f; else if (newLane > lane) lastDir = 1.0f;
        lane = newLane;
        if (state == PlayerState::RUN) { state = PlayerState::MOVE; frameNum = 0; }
    }
}

void Player::reset() { lane = 1; y = GROUND_Y; vY = 0.f; state = PlayerState::RUN; frameNum = 0; animTimer = 0.f; lastDir = 1.0f; }

void Player::update(float dt) {
    animTimer += dt;
    sprite.setScale({lastDir * 0.8f, 0.8f});

    float targetX = PLAYER_LANES_END_X[lane];
    float currX = sprite.getPosition().x;
    sprite.setPosition({currX + (targetX - currX) * 0.15f, y});

    std::vector<sf::IntRect>* currentList = &runFrames;
    float speed = 0.12f;

    if (state == PlayerState::JUMP) {
        currentList = &jumpFrames;
        speed = 0.15f;
        vY += 0.4f;
        y += vY;
        if (y >= GROUND_Y) { y = GROUND_Y; vY = 0.f; state = PlayerState::RUN; }
    }
    else if (state == PlayerState::MOVE) {
        currentList = &moveFrames;
        speed = 0.08f;
    }
    else if (state == PlayerState::DIE) {
        currentList = &dieFrames;
        speed = 0.15f;
    }

    if (animTimer > speed) {
        frameNum++;
        if (frameNum >= (int)currentList->size()) {
            if (state == PlayerState::DIE) frameNum = (int)currentList->size() - 1;
            else if (state == PlayerState::MOVE) { state = PlayerState::RUN; frameNum = 0; }
            else frameNum = 0;
        }
        animTimer = 0.f;
    }
    if (frameNum < (int)currentList->size()) updateSpriteRect((*currentList)[frameNum]);
}
