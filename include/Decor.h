#ifndef DECOR_H
#define DECOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Constants.h"
#include "SideBuilding.h"

struct SpecialEvent {
    float triggerP;
    const sf::Texture* tex;
    bool isRight;
    bool done = false;
};

class Decor {
public:
    sf::Sprite sky;
    sf::Sprite opera;
    std::vector<const sf::Texture*> buildingTextures;
    std::vector<std::unique_ptr<SideBuilding>> buildings;
    float timer = 0.f;
    const float targetTime = 90.f;
    std::vector<SpecialEvent> specialTimeline;
    sf::Clock clockL, clockR;

    Decor(const sf::Texture& tSky, const sf::Texture& tOpera,
          const std::vector<const sf::Texture*>& tBuildings,
          const sf::Texture& tArc, const sf::Texture& tGalerie,
          const sf::Texture& tNotreDame, const sf::Texture& tMoulin);

    void spawnInitialBuildings();
    void update(float dt, GameState state);
    void reset();
    void draw(sf::RenderWindow& window);
};

#endif