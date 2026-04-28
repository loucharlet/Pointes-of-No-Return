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
    : scenes(scenes_), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_) {
    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    if (auto* tex = AssetLoader::getTexture("costumes_bg.png")) {
        bg = std::make_unique<sf::Sprite>(*tex);
        AssetLoader::scaleToCoverLeft(*bg, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    if (auto* tex = AssetLoader::getTexture("BACK.png")) {
        backBtn = std::make_unique<sf::Sprite>(*tex);
        backBtn->setScale({0.05f, 0.05f});
        backBtn->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    if (auto* tex = AssetLoader::getTexture("costuventory.png")) {
        costuventoryBtn = std::make_unique<sf::Sprite>(*tex);
        costuventoryBtn->setScale({0.4f, 0.4f});
        auto b = costuventoryBtn->getLocalBounds();
        costuventoryBtn->setOrigin({b.size.x / 2.f, b.size.y});
        costuventoryBtn->setPosition({WINDOW_WIDTH / 2.f + 200.f, WINDOW_HEIGHT - 105.f});
    }

    if (auto* tex = AssetLoader::getTexture("mouse_spritesheet.png")) {
        mouseSprite = std::make_unique<sf::Sprite>(*tex);
        mouseSprite->setTextureRect(sf::IntRect({0, 0}, {MOUSE_FRAME_WIDTH, MOUSE_FRAME_HEIGHT}));
        mouseSprite->setScale({0.35f, 0.35f});
        mouseSprite->setPosition({WINDOW_WIDTH / 2.f + 280.f, WINDOW_HEIGHT - 180.f});
    }

    overlay.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    panel.setSize({800.f, 500.f});
    panel.setFillColor(sf::Color(30, 30, 40, 230));
    panel.setOutlineThickness(5.f);
    panel.setOutlineColor(sf::Color::White);
    panel.setOrigin({panel.getSize().x / 2.f, panel.getSize().y / 2.f});
    panel.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});

    struct Config { std::string id; std::string img; int req; };
    std::vector<Config> configs = {
        {"ballet", "modal_ballet.png", 0},
        {"ladybug", "modal_ladybug.png", 50},
        {"rythmics", "modal_rythmics.png", 100}
    };

    // RESTORED scale and spacing from d391cec
    float stepX = 320.f;
    float startX = WINDOW_WIDTH / 2.f - stepX;
    float yPos = WINDOW_HEIGHT / 2.f - 60.f;

    for (int i = 0; i < (int)configs.size(); ++i) {
        CostumeUI cui;
        cui.id = configs[i].id;
        cui.requiredCostomables = configs[i].req;
        
        if (auto* tex = AssetLoader::getTexture(configs[i].img)) {
            cui.modalSprite = std::make_unique<sf::Sprite>(*tex);
            cui.modalSprite->setScale({0.16f, 0.16f}); // RESTORED
            auto b = cui.modalSprite->getLocalBounds();
            cui.modalSprite->setOrigin({b.size.x / 2.f, b.size.y / 2.f});
            cui.modalSprite->setPosition({startX + i * stepX, yPos});
        }
        
        if (auto* tex = AssetLoader::getTexture("verrouille.png")) {
            cui.btnSprite = std::make_unique<sf::Sprite>(*tex);
            cui.btnSprite->setScale({0.08f, 0.08f}); // RESTORED
        }
        
        costumes.push_back(std::move(cui));
    }

    updateCostumesUI();
}

void CostumeState::updateCostumesUI() {
    float buttonYOffset = 130.f; // RESTORED
    for (auto& c : costumes) {
        c.isUnlocked = (saveData.costomables >= c.requiredCostomables);
        
        std::string texName = "verrouille.png";
        if (c.isUnlocked) {
            texName = (saveData.equippedCostume == c.id) ? "equipe.png" : "desequipe.png";
        }
        
        if (auto* tex = AssetLoader::getTexture(texName)) {
            if (!c.btnSprite) {
                c.btnSprite = std::make_unique<sf::Sprite>(*tex);
            } else {
                c.btnSprite->setTexture(*tex, true);
            }
            
            c.btnSprite->setScale({0.08f, 0.08f}); // RESTORED
            auto b = c.btnSprite->getLocalBounds();
            c.btnSprite->setOrigin({b.size.x / 2.f, b.size.y / 2.f});
            if (c.modalSprite) {
                c.btnSprite->setPosition({c.modalSprite->getPosition().x, c.modalSprite->getPosition().y + buttonYOffset});
            }
        } else if (c.btnSprite) {
            c.btnSprite->setScale({0.f, 0.f});
        }
    }
}

void CostumeState::equip(const std::string& id) {
    saveData.equippedCostume = id;
    SaveManager::save(saveData, slotPath);
    updateCostumesUI();
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
            for (auto& c : costumes) {
                if (c.btnSprite && c.btnSprite->getGlobalBounds().contains(mPos)) {
                    if (c.isUnlocked && saveData.equippedCostume != c.id) {
                        equip(c.id);
                    }
                    return;
                }
            }
            showingCostumes = false;
            return;
        }

        if (backBtn && backBtn->getGlobalBounds().contains(mPos)) {
            scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
            return;
        }

        if (costuventoryBtn && costuventoryBtn->getGlobalBounds().contains(mPos)) {
            showingCostumes = true;
            return;
        }
    }

    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            if (showingCostumes) showingCostumes = false;
            else scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
        }
    }
}

void CostumeState::update(float dt) {
    mouseTimer += dt;
    if (mouseTimer >= MOUSE_FRAME_DURATION) {
        mouseTimer -= MOUSE_FRAME_DURATION;
        mouseFrame = (mouseFrame + 1) % MOUSE_FRAME_COUNT;
        if (mouseSprite) {
            mouseSprite->setTextureRect(sf::IntRect({mouseFrame * MOUSE_FRAME_WIDTH, 0}, {MOUSE_FRAME_WIDTH, MOUSE_FRAME_HEIGHT}));
        }
    }
}

void CostumeState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(15, 15, 20));
    if (bg) window.draw(*bg);
    if (backBtn) window.draw(*backBtn);

    if (costuventoryBtn) window.draw(*costuventoryBtn);

    if (!showingCostumes && mouseSprite) {
        window.draw(*mouseSprite);
    }

    if (showingCostumes) {
        for (auto& c : costumes) {
            if (c.modalSprite) window.draw(*c.modalSprite);
            if (c.btnSprite && c.btnSprite->getScale().x > 0.01f) {
                window.draw(*c.btnSprite);
            }
        }
    }

    settingsUI.draw(window);
}
