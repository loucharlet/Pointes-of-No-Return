#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Constants.h"

class Player {
public:
    sf::Sprite sprite;
    int lane = 1;
    float y = GROUND_Y;
    float vY = 0.f;
    PlayerState state = PlayerState::RUN;
    float jumpDelay = 0.f;
    int frameNum = 0;
    float animTimer = 0.f;
    float lastDir = 1.0f;

    // animations liste
    std::vector<sf::IntRect> runFrames;
    std::vector<sf::IntRect> jumpFrames;
    std::vector<sf::IntRect> moveFrames;
    std::vector<sf::IntRect> dieFrames;

    Player(const sf::Texture& tex);

    void updateSpriteRect(sf::IntRect rect);
    void jump();
    void changeLane(int newLane);
    void reset();
    void update(float dt);
};

#endif