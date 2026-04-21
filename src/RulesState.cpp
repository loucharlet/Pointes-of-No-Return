#include "RulesState.h"
#include "SceneManager.h"
#include "Menu2State.h"
#include "AssetLoader.h"
#include "Constants.h"

std::unique_ptr<Scene> createRules(SceneManager* scenes, const Save* save, int slotIndex, const std::string& slotPath) {
    Save s = save ? *save : Save();
    return std::make_unique<RulesState>(scenes, s, slotIndex, slotPath);
}

RulesState::RulesState(SceneManager* scenes_, const Save& save, int slotIndex_, const std::string& slotPath_)
    : scenes(scenes_), saveData(save), slotIndex(slotIndex_), slotPath(slotPath_)
{
    AssetLoader::loadFont(font, "police_futura.ttf");
    settingsUI.init(font);
    settingsUI.setGameActionsEnabled(false);

    if (AssetLoader::loadTexture(rulesTex, "bookrules.png")) {
        rulesSprite = std::make_unique<sf::Sprite>(rulesTex);
        float scaleX = WINDOW_WIDTH / (float)rulesTex.getSize().x;
        float scaleY = (WINDOW_HEIGHT + 80.f) / (float)rulesTex.getSize().y;
        rulesSprite->setScale(sf::Vector2f(scaleX, scaleY));
        rulesSprite->setPosition({0.f, -120.f});
    }

    if (AssetLoader::loadTexture(backTex, "BACK.png")) {
        backBtn = std::make_unique<sf::Sprite>(backTex);
        backBtn->setScale({0.05f, 0.05f});
        backBtn->setPosition({20.f, WINDOW_HEIGHT - 130.f});
    }

    langBtn.setSize({190.f, 36.f});
    langBtn.setFillColor(sf::Color(80, 40, 20, 200));
    langBtn.setOutlineColor(sf::Color(160, 120, 60));
    langBtn.setOutlineThickness(1.5f);
    langBtn.setPosition({390.f, WINDOW_HEIGHT - 75.f});
}

void RulesState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
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

        if (langBtn.getGlobalBounds().contains(mPos)) {
            showEnglish = !showEnglish;
            return;
        }
        if (backBtn && backBtn->getGlobalBounds().contains(mPos)) {
            scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
            return;
        }
        scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
    }
    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape)
            scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
    }
}

void RulesState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(0, 0, 0));
    if (rulesSprite) window.draw(*rulesSprite);
    float pageLeft = 350.f;
    float pageWidth = 345.f;
    float y = 130.f;

    auto drawCentered = [&](const std::string& str, unsigned int size, sf::Color color) {
        sf::Text t(font, str, size);
        t.setFillColor(color);
        float tw = t.getGlobalBounds().size.x;
        t.setPosition({pageLeft + (pageWidth - tw) / 2.f, y});
        window.draw(t);
        y += size + 8.f;
    };

    auto drawLeft = [&](const std::string& str, unsigned int size, sf::Color color) {
        sf::Text t(font, str, size);
        t.setFillColor(color);
        t.setPosition({pageLeft + 90.f, y});
        window.draw(t);
        y += size + 6.f;
    };

    sf::Color titreColor(180, 20, 40);
    sf::Color corpsColor(245, 240, 230);

    if (!showEnglish) {
        drawCentered("~ L'HISTOIRE ~", 15, titreColor);
        drawCentered("Le petit rat de l'Opera de Paris", 12, corpsColor);
        drawCentered("doit traverser les toits de la ville", 12, corpsColor);
        drawCentered("pour arriver a l'heure danser le ballet!", 12, corpsColor);
        y += 12.f;

        drawCentered("~ NIVEAU 1 ~", 15, titreColor);
        drawCentered("Evite les cheminees sur les toits.", 12, corpsColor);
        drawCentered("Ramasse les 4 collectibles !", 12, corpsColor);
        y += 8.f;

        drawCentered("COMMANDES", 14, titreColor);
        drawCentered("<-   ->   Changer de voie", 12, corpsColor);
        drawCentered("ESPACE   Sauter", 12, corpsColor);
        y += 12.f;

        drawCentered("~ EN SCENE ~", 15, titreColor);
        drawCentered("Debloque les ballets :", 12, corpsColor);
        drawCentered("Swan Lake (50)  Nutcracker (200)", 11, corpsColor);
        drawCentered("Sylphide (100)  Coppelia (250)", 11, corpsColor);
        drawCentered("Jeu de rythme en musique classique.", 12, corpsColor);
    } else {
        drawCentered("~ THE STORY ~", 15, titreColor);
        drawCentered("A young ballet student must race", 12, corpsColor);
        drawCentered("across the Parisian rooftops", 12, corpsColor);
        drawCentered("to make it on stage in time!", 12, corpsColor);
        y += 12.f;

        drawCentered("~ LEVEL 1 ~", 15, titreColor);
        drawCentered("Dodge chimneys on the rooftops.", 12, corpsColor);
        drawCentered("Collect the 4 items!", 12, corpsColor);
        y += 8.f;

        drawCentered("CONTROLS", 14, titreColor);
        drawCentered("<-   ->   Change lane", 12, corpsColor);
        drawCentered("SPACE   Jump", 12, corpsColor);
        y += 12.f;

        drawCentered("~ EN SCENE ~", 15, titreColor);
        drawCentered("Unlock ballets by collecting items:", 12, corpsColor);
        drawCentered("Swan Lake (50)  Nutcracker (200)", 11, corpsColor);
        drawCentered("Sylphide (100)  Coppelia (250)", 11, corpsColor);
        drawCentered("Rhythm game with classical music.", 12, corpsColor);
    }


    window.draw(langBtn);
    std::string langLabel = showEnglish ? "Passer en francais" : "Switch to English";
    sf::Text langTxt(font, langLabel, 14);
    langTxt.setFillColor(sf::Color(220, 200, 170));
    langTxt.setPosition({
        langBtn.getPosition().x + langBtn.getSize().x / 2.f - langTxt.getGlobalBounds().size.x / 2.f,
        langBtn.getPosition().y + 10.f
    });
    window.draw(langTxt);

    if (backBtn) window.draw(*backBtn);

    settingsUI.draw(window);
}