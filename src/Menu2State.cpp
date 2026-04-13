#include "Menu2State.h"
#include "SceneManager.h"
#include "LevelSelectState.h"
#include "DressingState.h"
#include "RulesState.h"
#include "Menu1State.h"
#include "GameplayState.h"
#include "AssetLoader.h"
#include <iostream>
#include "Constants.h"

Menu2State::Menu2State(SceneManager* manager, const Save& save, int slotIndex_, const std::string& slotPath_)
    : scenes(manager), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_)
{
    // load background from assets
    AssetLoader::loadTexture(bgTex, "menubg2.png");
    bg = std::make_unique<sf::Sprite>(bgTex);
    if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
        bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x, WINDOW_HEIGHT / (float)bgTex.getSize().y));
    }
    bg->setPosition({0.f, 0.f});

    AssetLoader::loadFont(font, "police_futura.ttf");
    // try load button images from assets
    AssetLoader::loadTexture(selTex, "Button_EnScene.png");
    AssetLoader::loadTexture(dressTex, "Button_Inventaire.png");
    AssetLoader::loadTexture(rulesTex, "rules.png");
    AssetLoader::loadTexture(backTex, "BACK.png");
    AssetLoader::loadTexture(level1Tex, "niveau1.png");


    float startY = 220.f;


    // create sprites if textures available
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
    if (rulesTex.getSize().x > 0) {
        rulesBtnSprite = std::make_unique<sf::Sprite>(rulesTex);
        rulesBtnSprite->setScale({0.12f, 0.12f});
    }
    if (backTex.getSize().x > 0) {
        backSprite = std::make_unique<sf::Sprite>(backTex);
        backSprite->setScale({0.05f, 0.05f});
    }
    // position sprites near text
    if (selSprite) selSprite->setPosition({385.f, WINDOW_HEIGHT -520.f});
    if (level1Sprite) level1Sprite->setPosition({65.f, WINDOW_HEIGHT - 350.f});
    if (dressSprite) dressSprite->setPosition({704.f, WINDOW_HEIGHT - 360.f});
    if (rulesBtnSprite) rulesBtnSprite->setPosition({30.f, WINDOW_HEIGHT - 780.f});
    // back button bottom left
    if (backSprite) backSprite->setPosition({20.f, WINDOW_HEIGHT - 130.f});

}

// convenience ctor
Menu2State::Menu2State(SceneManager* manager)
    : scenes(manager), saveData(), slotIndex(-1), slotPath("")
{
    // delegate to main ctor logic by calling the other ctor's init steps
    // simple re-use: load background and buttons similarly
    AssetLoader::loadTexture(bgTex, "menubg2.png");
    bg = std::make_unique<sf::Sprite>(bgTex);
    if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
        bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x, WINDOW_HEIGHT / (float)bgTex.getSize().y));
    }
    bg->setPosition({0.f,0.f});

    AssetLoader::loadFont(font, "police_futura.ttf");
    float startY = 220.f;

    AssetLoader::loadTexture(selTex, "Button_EnScene.png");
    AssetLoader::loadTexture(dressTex, "Button_Inventaire.png");
    AssetLoader::loadTexture(rulesTex, "rules.png");
    AssetLoader::loadTexture(backTex, "BACK.png");

    float btnScale = 0.15f;
    if (selTex.getSize().x>0) {
        selSprite = std::make_unique<sf::Sprite>(selTex);
        selSprite->setScale({btnScale, btnScale});
    }
    if (dressTex.getSize().x>0) {
        dressSprite = std::make_unique<sf::Sprite>(dressTex);
        dressSprite->setScale({btnScale, btnScale});
    }
    if (rulesTex.getSize().x>0) {
        rulesBtnSprite = std::make_unique<sf::Sprite>(rulesTex);
        rulesBtnSprite->setScale({0.12f, 0.12f});
    }
    if (backTex.getSize().x>0) {
        backSprite = std::make_unique<sf::Sprite>(backTex);
        backSprite->setScale({0.05f, 0.05f});
    }
    if (selSprite) selSprite->setPosition({395.f, WINDOW_HEIGHT - 460.f});
    if (dressSprite) dressSprite->setPosition({700.f, WINDOW_HEIGHT - 300.f});
    if (rulesBtnSprite) rulesBtnSprite->setPosition({80.f, WINDOW_HEIGHT - 290.f});
    if (backSprite) backSprite->setPosition({20.f, WINDOW_HEIGHT - 130.f});


}

void Menu2State::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;
    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);
        
        // If rules are showing, clicking anywhere closes them
        if (showingRules) {
            showingRules = false;
            return;
        }

        // Check each button sprite directly
        if (level1Sprite && level1Sprite->getGlobalBounds().contains(mPos)) {
            // directly start level 1
            scenes->setScene(std::make_unique<GameplayState>(scenes, 1, saveData, slotPath, slotIndex));
        }
        else if (selSprite && selSprite->getGlobalBounds().contains(mPos)) {
            scenes->setScene(createLevelSelect(scenes, &saveData, slotIndex, slotPath));
        }
        else if (dressSprite && dressSprite->getGlobalBounds().contains(mPos)) {
            scenes->setScene(createDressing(scenes, &saveData, slotIndex, slotPath));
        }
        else if (rulesBtnSprite && rulesBtnSprite->getGlobalBounds().contains(mPos)) {
            scenes->setScene(createRules(scenes, &saveData, slotIndex, slotPath));
        }
        else if (backSprite && backSprite->getGlobalBounds().contains(mPos)) {
            scenes->setScene(std::make_unique<Menu1State>(scenes));
        }
    }
}

void Menu2State::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(0,0,0));
    if (bg) window.draw(*bg);
    if (selSprite) window.draw(*selSprite);
    if (level1Sprite) window.draw(*level1Sprite);
    if (dressSprite) window.draw(*dressSprite);
    if (rulesBtnSprite) window.draw(*rulesBtnSprite);
    if (backSprite) window.draw(*backSprite);

    // Draw rules overlay if needed
    if (showingRules && rulesPageSprite) {
        window.draw(*rulesPageSprite);
    }
}