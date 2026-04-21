#include "Menu2State.h"
#include "SceneManager.h"
#include "LevelSelectState.h"
#include "DressingState.h"
#include "RulesState.h"
#include "CostumeState.h"
#include "Menu1State.h"
#include "GameplayState.h"
#include "AssetLoader.h"
#include <iostream>
#include "Constants.h"

void Menu2State::initCommon() {
    sf::Texture* idleTex = AssetLoader::getTexture("player_menu2_pink.png");
    if (!idleTex) return;

    idleFrames.clear();
    float w = 179.f;

    for(int i = 0; i < 6; ++i) {
        idleFrames.push_back(sf::IntRect({(int)(i * w), 0}, {179, 230}));
    }
    for(int i = 0; i < 6; ++i) {
        idleFrames.push_back(sf::IntRect({(int)(i * w), 237}, {179, 174}));
    }

    idleSprite = std::make_unique<sf::Sprite>(*idleTex);
    idleSprite->setScale({IDLE_SCALE, IDLE_SCALE});

    const auto& r0 = idleFrames[0];
    idleSprite->setTextureRect(r0);
    idleSprite->setOrigin({r0.size.x / 2.f, (float)r0.size.y});
    idleSprite->setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 50.f});
}

Menu2State::Menu2State(SceneManager* manager, const Save& save, int slotIndex_, const std::string& slotPath_)
    : scenes(manager), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_)
{
    if (auto* tex = AssetLoader::getTexture("menubg2.png")) {
        bg = std::make_unique<sf::Sprite>(*tex);
        bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)tex->getSize().x,
                                  WINDOW_HEIGHT / (float)tex->getSize().y));
        bg->setPosition({0.f, 0.f});
    }

    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    float btnScale = 0.15f;
    if (auto* tex = AssetLoader::getTexture("Button_EnScene.png")) {
        selSprite = std::make_unique<sf::Sprite>(*tex);
        selSprite->setScale({btnScale, btnScale});
        selSprite->setPosition({385.f, WINDOW_HEIGHT - 520.f});
    }
    if (auto* tex = AssetLoader::getTexture("niveau1.png")) {
        level1Sprite = std::make_unique<sf::Sprite>(*tex);
        level1Sprite->setScale({btnScale, btnScale});
        level1Sprite->setPosition({65.f, WINDOW_HEIGHT - 350.f});
    }
    if (auto* tex = AssetLoader::getTexture("Button_Inventaire.png")) {
        dressSprite = std::make_unique<sf::Sprite>(*tex);
        dressSprite->setScale({btnScale, btnScale});
        dressSprite->setPosition({704.f, WINDOW_HEIGHT - 360.f});
    }
    if (auto* tex = AssetLoader::getTexture("Button_Costume.png")) {
        costumeSprite = std::make_unique<sf::Sprite>(*tex);
        costumeSprite->setScale({btnScale, btnScale});
        costumeSprite->setPosition({704.f, WINDOW_HEIGHT - 680.f});
    }
    if (auto* tex = AssetLoader::getTexture("rules.png")) {
        rulesBtnSprite = std::make_unique<sf::Sprite>(*tex);
        rulesBtnSprite->setScale({0.12f, 0.12f});
        rulesBtnSprite->setPosition({30.f, WINDOW_HEIGHT - 780.f});
    }
    if (auto* tex = AssetLoader::getTexture("BACK.png")) {
        backSprite = std::make_unique<sf::Sprite>(*tex);
        backSprite->setScale({0.05f, 0.05f});
        backSprite->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    initCommon();
}

Menu2State::Menu2State(SceneManager* manager)
    : scenes(manager), saveData(), slotIndex(-1), slotPath("")
{
    if (auto* tex = AssetLoader::getTexture("menubg2.png")) {
        bg = std::make_unique<sf::Sprite>(*tex);
        bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)tex->getSize().x,
                                  WINDOW_HEIGHT / (float)tex->getSize().y));
        bg->setPosition({0.f, 0.f});
    }

    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    float btnScale = 0.15f;
    if (auto* tex = AssetLoader::getTexture("Button_EnScene.png")) {
        selSprite = std::make_unique<sf::Sprite>(*tex);
        selSprite->setScale({btnScale, btnScale});
        selSprite->setPosition({395.f, WINDOW_HEIGHT - 460.f});
    }
    if (auto* tex = AssetLoader::getTexture("Button_Inventaire.png")) {
        dressSprite = std::make_unique<sf::Sprite>(*tex);
        dressSprite->setScale({btnScale, btnScale});
        dressSprite->setPosition({700.f, WINDOW_HEIGHT - 300.f});
    }
    if (auto* tex = AssetLoader::getTexture("Button_Costume.png")) {
        costumeSprite = std::make_unique<sf::Sprite>(*tex);
        costumeSprite->setScale({btnScale, btnScale});
        costumeSprite->setPosition({700.f, WINDOW_HEIGHT - 680.f});
    }
    if (auto* tex = AssetLoader::getTexture("rules.png")) {
        rulesBtnSprite = std::make_unique<sf::Sprite>(*tex);
        rulesBtnSprite->setScale({0.12f, 0.12f});
        rulesBtnSprite->setPosition({80.f, WINDOW_HEIGHT - 290.f});
    }
    if (auto* tex = AssetLoader::getTexture("BACK.png")) {
        backSprite = std::make_unique<sf::Sprite>(*tex);
        backSprite->setScale({0.05f, 0.05f});
        backSprite->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    initCommon();
}

void Menu2State::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;
    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

        if (settingsUI.handleClick(
                mPos,
                [&]() { window.close(); },
                [&]() { scenes->setScene(std::make_unique<Menu1State>(scenes)); }
            )) return;

        if (showingRules) { showingRules = false; return; }

        if (level1Sprite && level1Sprite->getGlobalBounds().contains(mPos))
            scenes->setScene(std::make_unique<GameplayState>(scenes, 1, saveData, slotPath, slotIndex));
        else if (selSprite && selSprite->getGlobalBounds().contains(mPos))
            scenes->setScene(createLevelSelect(scenes, &saveData, slotIndex, slotPath));
        else if (dressSprite && dressSprite->getGlobalBounds().contains(mPos))
            scenes->setScene(createDressing(scenes, &saveData, slotIndex, slotPath));
        else if (costumeSprite && costumeSprite->getGlobalBounds().contains(mPos))
            scenes->setScene(createCostume(scenes, &saveData, slotIndex, slotPath));
        else if (rulesBtnSprite && rulesBtnSprite->getGlobalBounds().contains(mPos))
            scenes->setScene(createRules(scenes, &saveData, slotIndex, slotPath));
        else if (backSprite && backSprite->getGlobalBounds().contains(mPos))
            scenes->setScene(std::make_unique<Menu1State>(scenes));
    }
}

void Menu2State::update(float dt) {
    if (!idleSprite || idleFrames.empty()) return;

    idleTimer += dt;
    if (idleTimer >= IDLE_FRAME_DURATION) {
        idleTimer -= IDLE_FRAME_DURATION;
        idleFrame = (idleFrame + 1) % static_cast<int>(idleFrames.size());
        const auto& r = idleFrames[idleFrame];
        idleSprite->setTextureRect(r);
        idleSprite->setOrigin({r.size.x / 2.f, (float)r.size.y});
    }
}

void Menu2State::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(0, 0, 0));
    if (bg)             window.draw(*bg);
    if (idleSprite)     window.draw(*idleSprite);
    if (selSprite)      window.draw(*selSprite);
    if (level1Sprite)   window.draw(*level1Sprite);
    if (dressSprite)    window.draw(*dressSprite);
    if (costumeSprite)  window.draw(*costumeSprite);
    if (rulesBtnSprite) window.draw(*rulesBtnSprite);
    if (backSprite)     window.draw(*backSprite);

    if (showingRules && rulesPageSprite)
        window.draw(*rulesPageSprite);

    settingsUI.draw(window);
}
