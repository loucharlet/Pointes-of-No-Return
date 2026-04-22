#include "CostumeState.h"
#include "SceneManager.h"
#include "Menu2State.h"
#include "AssetLoader.h"
#include "Constants.h"

std::unique_ptr<Scene> createCostume(SceneManager* scenes, const Save* save, int slotIndex, const std::string& slotPath) {
    Save s = save ? *save : Save();
    return std::make_unique<CostumeState>(scenes, s, slotIndex, slotPath);
}

CostumeState::CostumeState(SceneManager* scenes_, const Save& save, int slotIndex_, const std::string& slotPath_)
    : scenes(scenes_), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_)
{
    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);


    if (AssetLoader::loadTexture(bgTex, "costumes_bg.png")) {
        bg = std::make_unique<sf::Sprite>(bgTex);
        if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
            bg->setScale(sf::Vector2f(WINDOW_WIDTH / (float)bgTex.getSize().x, WINDOW_HEIGHT / (float)bgTex.getSize().y));
        }
        bg->setPosition({0.f, 0.f});
    }

    if (AssetLoader::loadTexture(backTex, "BACK.png")) {
        backBtn = std::make_unique<sf::Sprite>(backTex);
        backBtn->setScale({0.05f, 0.05f});
        backBtn->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    openBtn.setSize({260.f, 60.f});
    openBtn.setFillColor(sf::Color(0, 0, 0, 160));
    openBtn.setOutlineThickness(3.f);
    openBtn.setOutlineColor(sf::Color::Black);
    openBtn.setPosition({WINDOW_WIDTH / 2.f - 130.f, WINDOW_HEIGHT - 130.f});

    openBtnTxt = std::make_unique<sf::Text>(font, "Costumes", 28);
    openBtnTxt->setFillColor(sf::Color::White);
    auto tb = openBtnTxt->getLocalBounds();
    openBtnTxt->setPosition({
        openBtn.getPosition().x + openBtn.getSize().x / 2.f - tb.size.x / 2.f,
        openBtn.getPosition().y + 14.f
    });

    panel.setSize({700.f, 520.f});
    panel.setFillColor(sf::Color(0, 0, 0, 190));
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(sf::Color::Black);
    panel.setOrigin({panel.getSize().x / 2.f, panel.getSize().y / 2.f});
    panel.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 20.f});

    panelTitle = std::make_unique<sf::Text>(font, "COSTUMES", 40);
    panelTitle->setFillColor(sf::Color::White);
    panelTitle->setOutlineColor(sf::Color::Black);
    panelTitle->setOutlineThickness(3.f);
    panelTitle->setPosition({WINDOW_WIDTH / 2.f - 150.f, 150.f});

    // mapping simple: 4 costumes liés aux 4 collectibles
    struct CostumeReq { std::string name; int have; int need; };
    std::vector<CostumeReq> req = {
        {"Swan Lake costume", saveData.coll1, 50},
        {"Nutcracker costume", saveData.coll2, 200},
        {"La Sylphide costume", saveData.coll3, 100},
        {"Coppelia costume", saveData.coll4, 250},
    };

    float y = 250.f;
    for (const auto& r : req) {
        bool unlocked = r.have >= r.need;
        std::string line = r.name + "  (" + std::to_string(r.have) + "/" + std::to_string(r.need) + ")  -  " + (unlocked ? "UNLOCKED" : "LOCKED");
        sf::Text t(font, line, 24);
        t.setFillColor(unlocked ? sf::Color(240, 240, 240) : sf::Color(200, 200, 200));
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(2.f);
        t.setPosition({WINDOW_WIDTH / 2.f - 290.f, y});
        costumeLines.push_back(t);
        y += 55.f;
    }
}

void CostumeState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
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

        if (showingCostumes) {
            // clic n'importe où ferme l'overlay
            showingCostumes = false;
            return;
        }

        if (backBtn && backBtn->getGlobalBounds().contains(mPos)) {
            scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
            return;
        }

        if (openBtn.getGlobalBounds().contains(mPos)) {
            showingCostumes = true;
            return;
        }
    }

    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
        }
    }
}

void CostumeState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(15, 15, 20));
    if (bg) window.draw(*bg);
    if (backBtn) window.draw(*backBtn);

    window.draw(openBtn);
    if (openBtnTxt) window.draw(*openBtnTxt);

    if (showingCostumes) {
        window.draw(panel);
        if (panelTitle) window.draw(*panelTitle);
        for (auto& t : costumeLines) window.draw(t);
    }

    settingsUI.draw(window);
}

