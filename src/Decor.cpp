#include "Decor.h"

Decor::Decor(const sf::Texture& tSky, const sf::Texture& tOpera,
             const std::vector<const sf::Texture*>& tBuildings,
             const sf::Texture& tArc, const sf::Texture& tGalerie,
             const sf::Texture& tNotreDame, const sf::Texture& tMoulin)
    : sky(tSky), opera(tOpera), buildingTextures(tBuildings) {

    specialTimeline.push_back({0.15f, &tArc, false});
    specialTimeline.push_back({0.35f, &tGalerie, false});
    specialTimeline.push_back({0.60f, &tNotreDame, true});
    specialTimeline.push_back({0.85f, &tMoulin, true});

    sky.setPosition({0.f, 0.f});
    sf::FloatRect bOpera = opera.getLocalBounds();
    opera.setOrigin({bOpera.size.x / 2.f, bOpera.size.y / 2.f});
    opera.setPosition({500.f, HORIZON_Y + 50.f});
    opera.setScale({0.1f, 0.1f});
    spawnInitialBuildings();
}

void Decor::spawnInitialBuildings() {
    buildings.clear();
    for (int i = 2; i >= 0; i--) {
        float p = (float)i * 0.33f;
        buildings.push_back(std::make_unique<SideBuilding>(*buildingTextures[rand() % buildingTextures.size()], false, p));
        buildings.push_back(std::make_unique<SideBuilding>(*buildingTextures[rand() % buildingTextures.size()], true, p));
    }
}

void Decor::update(float dt, GameState state) {
    if (state == GameState::PLAYING) {
        timer += dt;
        if (timer > targetTime) timer = targetTime;

        float pLevel = timer / targetTime;
        float opS = 0.1f + (pLevel * 0.3f);
        opera.setScale({opS, opS});
        opera.setPosition({500.f, (HORIZON_Y + 35.f) - (pLevel * 200.f)});

        SpecialEvent* currentEv = nullptr;
        bool monL = false, monR = false;

        for (auto& ev : specialTimeline) {
            if (!ev.done) {
                if (pLevel >= (ev.triggerP - 0.03f)) {
                    if (ev.isRight) monR = true; else monL = true;
                }
                if (pLevel >= ev.triggerP) currentEv = &ev;
                break;
            }
        }

        if (currentEv) {
            sf::Clock& tClock = currentEv->isRight ? clockR : clockL;
            if (tClock.getElapsedTime().asSeconds() > 8.0f) {
                buildings.push_back(std::make_unique<SideBuilding>(*currentEv->tex, currentEv->isRight, 0.f));
                currentEv->done = true;
                tClock.restart();
            }
        }

        if (!monL && clockL.getElapsedTime().asSeconds() > 3.5f) {
            buildings.push_back(std::make_unique<SideBuilding>(*buildingTextures[rand() % buildingTextures.size()], false, 0.f));
            clockL.restart();
        }
        if (!monR && clockR.getElapsedTime().asSeconds() > 3.8f) {
            buildings.push_back(std::make_unique<SideBuilding>(*buildingTextures[rand() % buildingTextures.size()], true, 0.f));
            clockR.restart();
        }

        for (auto it = buildings.begin(); it != buildings.end(); ) {
            if ((*it)->update(gameSpeed)) it = buildings.erase(it); else ++it;
        }
    }
}

void Decor::reset() {
    timer = 0.f;
    for (auto& ev : specialTimeline) ev.done = false;
    opera.setScale({0.1f, 0.1f});
    spawnInitialBuildings();
}

void Decor::draw(sf::RenderWindow& window) {
    window.draw(sky);
    window.draw(opera);
    for (auto it = buildings.rbegin(); it != buildings.rend(); ++it) {
        window.draw((*it)->sprite);
    }
}