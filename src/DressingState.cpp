#include "DressingState.h"
#include "SceneManager.h"
#include "Menu2State.h"
#include "Constants.h"
#include "AssetLoader.h"
#include <iostream>

std::unique_ptr<Scene> createDressing(SceneManager* scenes, const Save* save, int slotIndex, const std::string& slotPath) {
    Save s = save ? *save : Save();
    return std::make_unique<DressingState>(scenes, s, slotIndex, slotPath);
}

DressingState::DressingState(SceneManager* scenes_, const Save& save, int slotIndex_, const std::string& slotPath_)
    : scenes(scenes_), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_)
{
    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);
    
    // bg: backstagesbg.png
    if (AssetLoader::loadTexture(bgTex, "backstagesbg.png")) {
        dressingBg = std::make_unique<sf::Sprite>(bgTex);
        dressingBg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x, WINDOW_HEIGHT / (float)bgTex.getSize().y));
    }

    // back Button
    if (AssetLoader::loadTexture(backTex, "BACK.png")) {
        backBtn = std::make_unique<sf::Sprite>(backTex);
        backBtn->setScale({0.05f, 0.05f});
        backBtn->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    // Small Inventory Button
    if (AssetLoader::loadTexture(invTex, "inventaire.png")) {
        invBtn = std::make_unique<sf::Sprite>(invTex);
        invBtn->setScale({0.26f, 0.26f});
        invBtn->setPosition({WINDOW_WIDTH - 550.f, WINDOW_HEIGHT - 330.f});
    }

    // large Inventory Overlay
    if (AssetLoader::loadTexture(invOpenTex, "inventaireOpen.png")) {
        invOpenSprite = std::make_unique<sf::Sprite>(invOpenTex);
        invOpenSprite->setScale(sf::Vector2f(WINDOW_WIDTH / (float)invOpenTex.getSize().x, WINDOW_HEIGHT / (float)invOpenTex.getSize().y * 1.5f ));
        invOpenSprite->setPosition({WINDOW_WIDTH -1000.f, WINDOW_HEIGHT -1000.f});
    }

    // Load collectible icons
    AssetLoader::loadTexture(c1Tex, "coll1.png");
    AssetLoader::loadTexture(c2Tex, "coll2.png");
    AssetLoader::loadTexture(c3Tex, "coll3.png");
    AssetLoader::loadTexture(c4Tex, "coll4.png");
}

void DressingState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;
    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

        if (settingsUI.handleClick(
                mPos,
                [&]() { window.close(); },
                [&]() { scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath)); }
            )) {
            return;
        }
        
        // si ouvert, clic pour fermer
        if (showingInventory) {
            showingInventory = false;
            return;
        }

        //back button
        if (backBtn && backBtn->getGlobalBounds().contains(mPos)) {
             scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
             return;
        }

        // inventory button
        if (invBtn && invBtn->getGlobalBounds().contains(mPos)) {
            showingInventory = true;
        }
    }
}

void DressingState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(12,12,20));
    if (dressingBg) window.draw(*dressingBg);
    if (backBtn) window.draw(*backBtn);
    
    if (!showingInventory && invBtn) {
        window.draw(*invBtn);
    }

    if (showingInventory && invOpenSprite) {
        window.draw(*invOpenSprite);

        struct CollData { sf::Texture& tex; int count; int target; };
        std::vector<CollData> data = {
            {c1Tex, saveData.coll1, 50},
            {c2Tex, saveData.coll2, 200},
            {c3Tex, saveData.coll3, 100},
            {c4Tex, saveData.coll4, 250}
        };

        float boxW = 280.f;
        float boxH = 140.f;
        float spacingX = 30.f;
        float spacingY = 40.f;

        // startX diminue -> + a gauche
        float startX = WINDOW_WIDTH * 0.18f;
        float startY = WINDOW_HEIGHT * 0.28f;
        int cols = 2;

        for (int i = 0; i < (int)data.size(); ++i) {
            int row = i / cols;
            int col = i % cols;
            float x = startX + col * (boxW + spacingX);
            float y = startY + row * (boxH + spacingY);

            //fond noir clear
            sf::RectangleShape slot(sf::Vector2f(boxW, boxH));
            slot.setPosition({x, y});
            slot.setFillColor(sf::Color(0, 0, 0, 130));
            slot.setOutlineThickness(2.f);
            slot.setOutlineColor(sf::Color(0,0,0)); // contour rose léger
            window.draw(slot);

            // icon coll
            if (data[i].tex.getSize().x > 0) {
                sf::Sprite s(data[i].tex);
                auto ts = data[i].tex.getSize();
                float sc = (boxH * 0.65f) / (float)ts.y;
                s.setScale({sc, sc});

                float iconH = ts.y * sc;
                s.setPosition({x + 15.f, y + (boxH - iconH) / 2.f});
                window.draw(s);
            }

            //text
            std::string msg = std::to_string(data[i].count) + "/" + std::to_string(data[i].target);
            sf::Text txt(font, msg, 28); // police plus grosse
            txt.setFillColor(sf::Color::White); // blanc sur fond noir = lisibilité max

            // centrage texte
            sf::FloatRect tb = txt.getLocalBounds();
            txt.setPosition({x + boxW - tb.size.x - 20.f, y + (boxH - tb.size.y) / 2.f - 5.f});
            window.draw(txt);
        }
    }

    settingsUI.draw(window);
}