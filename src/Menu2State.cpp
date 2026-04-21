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

// ─────────────────────────────────────────────────────────────
//  initCommon
// ─────────────────────────────────────────────────────────────
void Menu2State::initCommon() {
    if (!AssetLoader::loadTexture(idleTex, "player_menu2_pink.png")) return;

    idleFrames.clear();
    float w = 179.f;

    // --- ligne 1 : on prend large en bas pour pas couper (h=230) ---
    for(int i = 0; i < 6; ++i) {
        idleFrames.push_back(sf::IntRect({(int)(i * w), 0}, {179, 230}));
    }

    // --- ligne 2 : on ajuste frame par frame car c'est là que ça saute ---
    // startY plus bas (260) et hauteur plus courte (200) pour compenser le décalage
    for(int i = 0; i < 6; ++i) {
        idleFrames.push_back(sf::IntRect({(int)(i * w), 237}, {179, 174}));
    }

    idleSprite = std::make_unique<sf::Sprite>(idleTex);
    idleSprite->setScale({IDLE_SCALE, IDLE_SCALE});

    const auto& r0 = idleFrames[0];
    idleSprite->setTextureRect(r0);

    // l'origine au milieu des pieds
    idleSprite->setOrigin({r0.size.x / 2.f, (float)r0.size.y});
    // position sur l'écran
    idleSprite->setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 50.f});
}

// ─────────────────────────────────────────────────────────────
//  Constructeur principal
// ─────────────────────────────────────────────────────────────
Menu2State::Menu2State(SceneManager* manager, const Save& save, int slotIndex_, const std::string& slotPath_)
    : scenes(manager), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_)
{
    AssetLoader::loadTexture(bgTex, "menubg2.png");
    bg = std::make_unique<sf::Sprite>(bgTex);
    if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0)
        bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x,
                                  WINDOW_HEIGHT / (float)bgTex.getSize().y));
    bg->setPosition({0.f, 0.f});

    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    AssetLoader::loadTexture(selTex,     "Button_EnScene.png");
    AssetLoader::loadTexture(dressTex,   "Button_Inventaire.png");
    AssetLoader::loadTexture(costumeTex, "Button_Costume.png");
    AssetLoader::loadTexture(rulesTex,   "rules.png");
    AssetLoader::loadTexture(backTex,    "BACK.png");
    AssetLoader::loadTexture(level1Tex,  "niveau1.png");

    float btnScale = 0.15f;
    if (selTex.getSize().x > 0) {
        selSprite = std::make_unique<sf::Sprite>(selTex);
        selSprite->setScale({btnScale, btnScale});
    }
    if (level1Tex.getSize().x > 0) {
        level1Sprite = std::make_unique<sf::Sprite>(level1Tex);
        level1Sprite->setScale({btnScale, btnScale});
    }
    if (dressTex.getSize().x > 0) {
        dressSprite = std::make_unique<sf::Sprite>(dressTex);
        dressSprite->setScale({btnScale, btnScale});
    }
    if (costumeTex.getSize().x > 0) {
        costumeSprite = std::make_unique<sf::Sprite>(costumeTex);
        costumeSprite->setScale({btnScale, btnScale});
    }
    if (rulesTex.getSize().x > 0) {
        rulesBtnSprite = std::make_unique<sf::Sprite>(rulesTex);
        rulesBtnSprite->setScale({0.12f, 0.12f});
    }
    if (backTex.getSize().x > 0) {
        backSprite = std::make_unique<sf::Sprite>(backTex);
        backSprite->setScale({0.05f, 0.05f});
    }

    if (selSprite)      selSprite->setPosition({385.f,  WINDOW_HEIGHT - 520.f});
    if (level1Sprite)   level1Sprite->setPosition({65.f, WINDOW_HEIGHT - 350.f});
    if (dressSprite)    dressSprite->setPosition({704.f, WINDOW_HEIGHT - 360.f});
    if (costumeSprite)  costumeSprite->setPosition({704.f, WINDOW_HEIGHT - 680.f});
    if (rulesBtnSprite) rulesBtnSprite->setPosition({30.f, WINDOW_HEIGHT - 780.f});
    if (backSprite)     backSprite->setPosition({20.f,  WINDOW_HEIGHT - 130.f});

    initCommon();
}

// ─────────────────────────────────────────────────────────────
//  Convenience ctor
// ─────────────────────────────────────────────────────────────
Menu2State::Menu2State(SceneManager* manager)
    : scenes(manager), saveData(), slotIndex(-1), slotPath("")
{
    AssetLoader::loadTexture(bgTex, "menubg2.png");
    bg = std::make_unique<sf::Sprite>(bgTex);
    if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0)
        bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x,
                                  WINDOW_HEIGHT / (float)bgTex.getSize().y));
    bg->setPosition({0.f, 0.f});

    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    AssetLoader::loadTexture(selTex,     "Button_EnScene.png");
    AssetLoader::loadTexture(dressTex,   "Button_Inventaire.png");
    AssetLoader::loadTexture(costumeTex, "Button_Costume.png");
    AssetLoader::loadTexture(rulesTex,   "rules.png");
    AssetLoader::loadTexture(backTex,    "BACK.png");

    float btnScale = 0.15f;
    if (selTex.getSize().x > 0) {
        selSprite = std::make_unique<sf::Sprite>(selTex);
        selSprite->setScale({btnScale, btnScale});
    }
    if (dressTex.getSize().x > 0) {
        dressSprite = std::make_unique<sf::Sprite>(dressTex);
        dressSprite->setScale({btnScale, btnScale});
    }
    if (costumeTex.getSize().x > 0) {
        costumeSprite = std::make_unique<sf::Sprite>(costumeTex);
        costumeSprite->setScale({btnScale, btnScale});
    }
    if (rulesTex.getSize().x > 0) {
        rulesBtnSprite = std::make_unique<sf::Sprite>(rulesTex);
        rulesBtnSprite->setScale({0.12f, 0.12f});
    }
    if (backTex.getSize().x > 0) {
        backSprite = std::make_unique<sf::Sprite>(backTex);
        backSprite->setScale({0.05f, 0.05f});
    }

    if (selSprite)      selSprite->setPosition({395.f,  WINDOW_HEIGHT - 460.f});
    if (dressSprite)    dressSprite->setPosition({700.f, WINDOW_HEIGHT - 300.f});
    if (costumeSprite)  costumeSprite->setPosition({700.f, WINDOW_HEIGHT - 680.f});
    if (rulesBtnSprite) rulesBtnSprite->setPosition({80.f,  WINDOW_HEIGHT - 290.f});
    if (backSprite)     backSprite->setPosition({20.f,  WINDOW_HEIGHT - 130.f});

    initCommon();
}

// ─────────────────────────────────────────────────────────────
//  handleEvent
// ─────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────
//  update — avance la frame, recale l'origine sur le bas exact
// ─────────────────────────────────────────────────────────────
void Menu2State::update(float dt) {
    if (!idleSprite || idleFrames.empty()) return;

    idleTimer += dt;
    if (idleTimer >= IDLE_FRAME_DURATION) {
        idleTimer -= IDLE_FRAME_DURATION;

        // on passe à la frame suivante
        idleFrame = (idleFrame + 1) % static_cast<int>(idleFrames.size());

        const auto& r = idleFrames[idleFrame];
        idleSprite->setTextureRect(r);

        // IMPORTANT : on recalcule l'origine à chaque fois
        // au cas où les tailles de Rect diffèrent un peu
        idleSprite->setOrigin({r.size.x / 2.f, (float)r.size.y});
    }
}

// ─────────────────────────────────────────────────────────────
//  draw
// ─────────────────────────────────────────────────────────────
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