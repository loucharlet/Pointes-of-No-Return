#include "LevelSelectState.h"
#include "SceneManager.h"
#include "GameplayState.h"
#include "Menu2State.h"
#include <iostream>

std::unique_ptr<Scene> createLevelSelect(SceneManager* scenes, const Save* saveOverride, int slotIndex, const std::string& slotPath) {
    return std::make_unique<LevelSelectState>(scenes, saveOverride, slotIndex, slotPath);
}

LevelSelectState::LevelSelectState(SceneManager* manager, const Save* saveOverride, int slotIndex_, const std::string& slotPath_)
    : scenes(manager), slotIndex(slotIndex_), slotPath(slotPath_)
{
    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);
    if (saveOverride) save = *saveOverride;
    else SaveManager::load(save);

    if (AssetLoader::loadTexture(bgTex, "dressingbg.png")) {
        bg = std::make_unique<sf::Sprite>(bgTex);
        if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
            bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x, WINDOW_HEIGHT / (float)bgTex.getSize().y));
        }
    }

    if (AssetLoader::loadTexture(backTex, "BACK.png")) {
        backBtn = std::make_unique<sf::Sprite>(backTex);
        backBtn->setScale({0.05f, 0.05f});
        backBtn->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    // seuils réels par ballet — coll1=SwanLake(50), coll2=Nutcracker(200), coll3=Sylphide(100), coll4=Coppelia(250)
    const std::vector<int> counts = {save.coll1, save.coll4, save.coll2, save.coll3};
    const std::vector<int> REQ    = {50,         250,        200,        100};

    const std::vector<std::string> unlockedNames = {"logoSwanLake.png", "logoCoppelia.png", "logoNutcracker.png", "logoSylphide.png"};
    const std::vector<std::string> lockedNames   = {"logoLockedSwanLake.png", "logoLockedCoppelia.png", "logoLockedNutcracker.png", "logoLockedSylphide.png"};

    for (int i = 0; i < 4; ++i) {
        sf::Texture u, l;
        AssetLoader::loadTexture(u, unlockedNames[i]);
        AssetLoader::loadTexture(l, lockedNames[i]);
        imageUnlocked.push_back(std::move(u));
        imageLocked.push_back(std::move(l));
    }


    float btnW = 300.f;
    float btnH = 200.f;
    const std::vector<sf::Vector2f> positions = {
        {95.f,  170.f},   // Swan Lake
        {320.f, 170.f},   // Coppelia
        {95.f,  410.f},   // Nutcracker
        {320.f, 390.f},   // Sylphide
    };

    for (int i = 0; i < 4; ++i) {
        bool unlocked = counts[i] >= REQ[i];
        levels.push_back({sf::FloatRect(positions[i], sf::Vector2f(btnW, btnH)), unlocked, "Ballet " + std::to_string(i+1)});
    }
}

void LevelSelectState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;
    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

        if (settingsUI.handleClick(
                mPos,
                [&]() { window.close(); },
                [&]() { scenes->setScene(std::make_unique<Menu2State>(scenes, save, slotIndex, slotPath)); }
            )) {
            return;
        }

        if (backBtn && backBtn->getGlobalBounds().contains(mPos)) {
            scenes->setScene(std::make_unique<Menu2State>(scenes, save, slotIndex, slotPath));
            return;
        }

        for (size_t i = 0; i < levels.size(); ++i) {
            if (levels[i].rect.contains(mPos)) {
                if (levels[i].unlocked) {
                    scenes->setScene(std::make_unique<GameplayState>(scenes, static_cast<int>(i) + 1, save, slotPath, slotIndex));
                } else {
                    std::cout << "Ballet locked: need more collectibles\n";
                }
                break;
            }
        }
    }
    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            scenes->setScene(std::make_unique<Menu2State>(scenes, save, slotIndex, slotPath));
        }
    }
}

void LevelSelectState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(10, 10, 15));
    if (bg) window.draw(*bg);
    if (backBtn) window.draw(*backBtn);

    for (size_t i = 0; i < levels.size(); ++i) {
        const auto& L = levels[i];


        if (i < imageUnlocked.size() && imageUnlocked[i].getSize().x > 0 && L.unlocked) {
            sf::Sprite s(imageUnlocked[i]);
            auto ts = imageUnlocked[i].getSize();
            float sc = std::min(L.rect.size.x / (float)ts.x, L.rect.size.y / (float)ts.y);
            s.setScale(sf::Vector2f(sc , sc ));
            s.setPosition({L.rect.position.x + (L.rect.size.x - ts.x*sc)/2.f,
                           L.rect.position.y + (L.rect.size.y - ts.y*sc)/2.f});
            window.draw(s);
        } else if (i < imageLocked.size() && imageLocked[i].getSize().x > 0 && !L.unlocked) {
            sf::Sprite s(imageLocked[i]);
            auto ts = imageLocked[i].getSize();
            float sc = std::min(L.rect.size.x / (float)ts.x, L.rect.size.y / (float)ts.y);
            s.setScale(sf::Vector2f(sc , sc ));
            s.setPosition({L.rect.position.x + (L.rect.size.x - ts.x*sc)/2.f,
                           L.rect.position.y + (L.rect.size.y - ts.y*sc)/2.f});
            window.draw(s);
        } else {

            std::string label = L.name.empty() ? ("Ballet " + std::to_string(i+1)) : L.name;
            sf::Text txt(font, label, 28);
            txt.setPosition({L.rect.position.x + 20.f, L.rect.position.y + 20.f});
            txt.setFillColor(L.unlocked ? sf::Color::Black : sf::Color(200,200,200));
            window.draw(txt);
            if (!L.unlocked) {
                sf::Text locked(font, "LOCKED", 20);
                locked.setPosition({L.rect.position.x + 16.f, L.rect.position.y + L.rect.size.y - 36.f});
                locked.setFillColor(sf::Color::White);
                window.draw(locked);
            }
        }
    }

    sf::Text info(font, "Esc to return", 16);
    info.setPosition({20.f, WINDOW_HEIGHT - 40.f});
    info.setFillColor(sf::Color::White);
    window.draw(info);

    settingsUI.draw(window);
}