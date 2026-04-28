#include "Constants.h"
#include "Menu1State.h"
#include "SceneManager.h"
#include "Menu2State.h"
#include "AssetLoader.h"
#include "SaveManager.h"
#include <iostream>
#include <algorithm>

Menu1State::Menu1State(SceneManager* manager)
    : scenes(manager)
{
    // Use exact filenames found in directory
    AssetLoader::loadTexture(bgTex1, "menubg1_.1png.png");
    AssetLoader::loadTexture(bgTex2, "menubg1_2.png");
    
    bg = std::make_unique<sf::Sprite>(bgTex1);
    AssetLoader::scaleToCoverCenter(*bg, WINDOW_WIDTH, WINDOW_HEIGHT);

    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    if (AssetLoader::loadTexture(backTex, "QUIT.png")) {
        backBtn = std::make_unique<sf::Sprite>(backTex);
        backBtn->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    if (AssetLoader::loadTexture(logoTex, "logo.png")) {
        logo = std::make_unique<sf::Sprite>(logoTex);
        logo->setScale({0.11f, 0.11f});
        logo->setPosition({WINDOW_WIDTH - 310.f, 10.f});
    }

    AssetLoader::loadTexture(slotBtnTex, "Button_empty.png");

    slotPaths = {"save_slot1.txt", "save_slot2.txt", "save_slot3.txt"};
    float startY = 180.f;
    float rectW = 600.f, rectH = 100.f; 
    float centerX = (WINDOW_WIDTH - rectW) / 2.f;

    for (int i = 0; i < 3; ++i) {
        Save s;
        bool ok = SaveManager::load(s, slotPaths[i]);
        std::string label;
        if (!ok) {
            label = "new game";
        } else {
            label = s.playerName.empty() ? "player" : s.playerName;
        }

        sf::FloatRect rect(sf::Vector2f(centerX, startY + i * 130.f), sf::Vector2f(rectW, rectH));
        slotRects.push_back(rect);

        sf::Text t(font, label, 25);
        sf::FloatRect tb = t.getLocalBounds();
        t.setOrigin({tb.size.x/2.f, tb.size.y/2.f});
        t.setPosition({rect.position.x + rect.size.x/2.f, rect.position.y + rect.size.y/2.f});
        t.setFillColor(sf::Color::White);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(2.f);
        slotsText.push_back(t);
    }
    if (slotBtnTex.getSize().x > 0) slotBtn = std::make_unique<sf::Sprite>(slotBtnTex);
}

void Menu1State::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;
    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

        if (settingsUI.handleClick(mPos, [&]() { window.close(); }, [&]() { scenes->setScene(std::make_unique<Menu1State>(scenes)); })) return;

        if (backBtn && backBtn->getGlobalBounds().contains(mPos)) {
            window.close();
            return;
        }

        for (size_t i = 0; i < slotRects.size(); ++i) {
            if (slotRects[i].contains(mPos)) {
                Save s;
                bool ok = SaveManager::load(s, slotPaths[i]);
                if (!ok) {
                    s.playerName = "player";
                    s.coll1 = 0; s.coll2 = 0; s.coll3 = 0; s.coll4 = 0;
                    s.costomables = 0;
                    s.highscore = 0;
                    s.levelReached = 0;
                    SaveManager::save(s, slotPaths[i]);
                }
                scenes->setScene(std::make_unique<Menu2State>(scenes, s, (int)i, slotPaths[i]));
                break;
            }
        }
    }
}

void Menu1State::update(float dt) {
    bgTimer += dt;
    if (bgTimer >= 0.5f) {
        bgTimer -= 0.5f;
        bgIndex = (bgIndex + 1) % 2;
        if (bg) {
            bg->setTexture(bgIndex == 0 ? bgTex1 : bgTex2);
            AssetLoader::scaleToCoverCenter(*bg, WINDOW_WIDTH, WINDOW_HEIGHT);
        }
    }
}

void Menu1State::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(0,0,0));
    if (bg) window.draw(*bg);
    if (logo) window.draw(*logo);
    if (backBtn) window.draw(*backBtn);

    for (size_t i = 0; i < slotRects.size(); ++i) {
        const auto& r = slotRects[i];
        if (slotBtn) {
            auto ts = slotBtnTex.getSize();
            if (ts.x > 0 && ts.y > 0) {
                float scaleX = r.size.x / (float)ts.x;
                float scaleY = r.size.y / (float)ts.y;
                float scale = std::min(scaleX * 1.5f, scaleY * 1.5f);
                slotBtn->setScale(sf::Vector2f(scale, scale));
                float drawW = ts.x * scale;
                float drawH = ts.y * scale;
                float posX = r.position.x + (r.size.x - drawW) / 2.f;
                float posY = r.position.y + (r.size.y - drawH) / 2.f;
                slotBtn->setPosition({posX, posY});
            } else {
                slotBtn->setPosition({r.position.x, r.position.y});
            }
            window.draw(*slotBtn);
        } else {
            sf::RectangleShape box({r.size.x, r.size.y});
            box.setPosition({r.position.x, r.position.y});
            box.setFillColor(sf::Color(60,60,80));
            box.setOutlineColor(sf::Color::Black);
            box.setOutlineThickness(2.f);
            window.draw(box);
        }
        if (i < slotsText.size()) window.draw(slotsText[i]);
    }
    settingsUI.draw(window);
}
