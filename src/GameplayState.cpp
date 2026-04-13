#include "GameplayState.h"
#include "SceneManager.h"
#include "Player.h"
#include "Decor.h"
#include "Road.h"
#include "Obstacle.h"
#include "Collectible.h"
#include "SideBuilding.h"
#include "GameOverUI.h"
#include "Menu2State.h"
#include <iostream>

struct Popup {
    sf::Sprite sprite;
    float timer;
    Popup(const sf::Texture& texture) : sprite(texture), timer(0.f) {}
};

GameplayState::GameplayState(SceneManager* scenes_, int levelId_, const Save& save_, const std::string& path_, int slotIndex_)
: levelId(levelId_), scenes(scenes_), saveData(save_), slotPath(path_), slotIndex(slotIndex_)
{
    // load assets
    std::cout << "GameplayState started for levelId=" << levelId << std::endl;
    AssetLoader::loadTexture(pTex, "player_lvl1_pink.png");
    AssetLoader::loadTexture(oTex, "obstacle.png");
    AssetLoader::loadTexture(texBtnReplay, "REPLAY.png");
    AssetLoader::loadTexture(texBtnQuit, "QUIT.png");
    AssetLoader::loadTexture(skyTex, "bg_sky.png");
    AssetLoader::loadTexture(texClouds, "clouds.png");
    AssetLoader::loadTexture(operaTex, "opéra.png");
    AssetLoader::loadTexture(roadTex, "roof_ground.png");
    AssetLoader::loadTexture(bTex1, "building1.png");
    AssetLoader::loadTexture(bTex2, "building2.png");
    AssetLoader::loadTexture(bTex3, "building3.png");
    AssetLoader::loadTexture(tArc, "arc_de_triomphe.png");
    AssetLoader::loadTexture(tGalerie, "galeries_lafayette.png");
    AssetLoader::loadTexture(tNotreDame, "notre_dame.png");
    AssetLoader::loadTexture(tMoulin, "moulin_rouge.png");
    AssetLoader::loadTexture(texSettings, "settings_icon.png");
    AssetLoader::loadTexture(texScoreIcon, "score_icon.png");
    AssetLoader::loadTexture(texSettingsbg, "settingsbg.png");
    AssetLoader::loadTexture(tLogo, "logo.png");
    AssetLoader::loadTexture(tFullInventory, "inventaireFull.png");
    AssetLoader::loadTexture(tFullInventoryBack, "inventaireFullBack.png");

    AssetLoader::loadTexture(collTex1, "coll1.png");
    AssetLoader::loadTexture(collTex2, "coll2.png");
    AssetLoader::loadTexture(collTex3, "coll3.png");
    AssetLoader::loadTexture(collTex4, "coll4.png");

    AssetLoader::loadTexture(popupTex1, "+1swanlake.png");
    AssetLoader::loadTexture(popupTex2, "+1nutcracker.png");
    AssetLoader::loadTexture(popupTex3, "+1sylphide.png");
    AssetLoader::loadTexture(popupTex4, "+1coppelia.png");

    levelMusic.openFromFile("./assets/audio/level_theme.ogg");
    levelMusic.setLooping(true);
    gameOverMusic.openFromFile("./assets/audio/gameover_theme.ogg");
    gameOverMusic.setLooping(true);
    levelMusic.play();

    bufCollec.loadFromFile("./assets/audio/collectibles.ogg");
    bufDeath.loadFromFile("./assets/audio/death.ogg");
    bufStart.loadFromFile("./assets/audio/gamestart.ogg");
    sfxCollec = std::make_unique<sf::Sound>(bufCollec);
    sfxDeath = std::make_unique<sf::Sound>(bufDeath);
    sfxStart = std::make_unique<sf::Sound>(bufStart);

    texVariations = {&bTex1, &bTex2, &bTex3};
    decor = new Decor(skyTex, operaTex, texVariations, tArc, tGalerie, tNotreDame, tMoulin, texClouds);
    ballerine = new Player(pTex);
    route = new Road(roadTex);
    ui = new GameOverUI();
    ui->load("gameover.png");

    AssetLoader::loadFont(font, "police_futura.ttf");
    // construct texts after font is loaded
    scoreText = std::make_unique<sf::Text>(font, "0", 25);
    scoreText->setFillColor(sf::Color::Black);
    scoreText->setOutlineColor(sf::Color::White);
    scoreText->setOutlineThickness(2.f);
    scoreText->setPosition({150.f, 44.f});

    spriteScoreIcon = std::make_unique<sf::Sprite>(texScoreIcon);
    spriteScoreIcon->setScale({0.57f, 0.57f});
    spriteScoreIcon->setPosition({17.f, 20.f});

    spriteSettings = std::make_unique<sf::Sprite>(texSettings);
    spriteSettings->setScale({0.35f, 0.35f});
    spriteSettings->setPosition({WINDOW_WIDTH - 80.f, 30.f});

    spriteLogo = std::make_unique<sf::Sprite>(tLogo);
    spriteLogo->setScale({0.075f, 0.075f});
    spriteLogo->setPosition({WINDOW_WIDTH - 240.f, 610.f});

    spriteFullIventory = std::make_unique<sf::Sprite>(tFullInventory);
    spriteFullIventory->setScale({0.13f, 0.13f});
    spriteFullIventory->setPosition({WINDOW_WIDTH - 250.f, 480.f});

    spriteFullIventoryBack = std::make_unique<sf::Sprite>(tFullInventoryBack);
    spriteFullIventoryBack->setScale({0.13f, 0.13f});
    spriteFullIventoryBack->setPosition({WINDOW_WIDTH - 250.f, 480.f});

    settingslil = sf::RectangleShape({160.f, 120.f});
    settingslil.setFillColor(sf::Color::Black);
    settingslil.setPosition({WINDOW_WIDTH - 180.f, 85.f});

    txtQuit = std::make_unique<sf::Text>(font, "Quit", 22);
    txtRestart = std::make_unique<sf::Text>(font, "Restart", 22);
    txtOpenSettings = std::make_unique<sf::Text>(font, "+ settings", 22);
    txtQuit->setPosition({WINDOW_WIDTH - 170.f, 95.f});
    txtRestart->setPosition({WINDOW_WIDTH - 170.f, 130.f});
    txtOpenSettings->setPosition({WINDOW_WIDTH - 170.f, 165.f});

    bigPanel = sf::RectangleShape({600.f, 500.f});
    bigPanel.setTexture(&texSettingsbg);
    bigPanel.setOutlineThickness(5.f);
    bigPanel.setOutlineColor(sf::Color::Black);
    bigPanel.setOrigin({300.f, 250.f});
    bigPanel.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 60.f});

    pTitle = std::make_unique<sf::Text>(font, "SETTINGS", 45);
    pMusic = std::make_unique<sf::Text>(font, "Music: ON", 30);
    pSfx = std::make_unique<sf::Text>(font, "SFX: ON", 30);
    pVol = std::make_unique<sf::Text>(font, "Volume: 100", 30);
    pClose = std::make_unique<sf::Text>(font, "X", 25);
    pTitle->setPosition({WINDOW_WIDTH/2.f - 100.f, 175.f});
    pMusic->setPosition({WINDOW_WIDTH/2.f - 100.f, 280.f});
    pSfx->setPosition({WINDOW_WIDTH/2.f - 100.f, 340.f});
    pVol->setPosition({WINDOW_WIDTH/2.f - 100.f, 400.f});
    pClose->setPosition({WINDOW_WIDTH/2.f + 270.f, 100.f}); pClose->setFillColor(sf::Color::Red);

    pTitle->setOutlineColor(sf::Color::Black);
    pTitle->setOutlineThickness(3.f);
    pMusic->setOutlineColor(sf::Color::Black); pMusic->setOutlineThickness(3.f);
    pSfx->setOutlineColor(sf::Color::Black); pSfx->setOutlineThickness(3.f);
    pVol->setOutlineColor(sf::Color::Black); pVol->setOutlineThickness(3.f);

    sReplay = std::make_unique<sf::Sprite>(texBtnReplay); sQuit = std::make_unique<sf::Sprite>(texBtnQuit);
    sReplay->setPosition({250.f, 400.f}); sQuit->setPosition({550.f, 400.f});

    collectibleTextures = {&collTex1, &collTex2, &collTex3, &collTex4};
}

GameplayState::~GameplayState() {
    delete decor; delete ballerine; delete route; delete ui;
}

void GameplayState::onEnter() {
    // nothing
}

void GameplayState::onExit() {
    // nothing
}

void GameplayState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (!event) return;

    if (event->is<sf::Event::Closed>()) {
        // main handles window close
    }

    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

        if (showSettingsPanel) {
            if (pClose && pClose->getGlobalBounds().contains(mPos)) {
                showSettingsPanel = false;
            }
            if (pMusic && pMusic->getGlobalBounds().contains(mPos)) {
                musicOn = !musicOn;
                if (musicOn) levelMusic.play(); else levelMusic.pause();
                pMusic->setString(musicOn ? "Music: ON" : "Music: OFF");
            }
            if (pVol && pVol->getGlobalBounds().contains(mPos)) {
                volume = (volume >= 100.f) ? 0.f : volume + 20.f;
                levelMusic.setVolume(volume);
                gameOverMusic.setVolume(volume);
                pVol->setString("Volume: " + std::to_string((int)volume));
            }
            if (pSfx && pSfx->getGlobalBounds().contains(mPos)) {
                sfxOn = !sfxOn;
                pSfx->setString(sfxOn ? "SFX: ON" : "SFX: OFF");
            }
        }
        else if (showSettingsMenu) {
            if (txtOpenSettings && txtOpenSettings->getGlobalBounds().contains(mPos)) {
                showSettingsPanel = true;
                showSettingsMenu = false;
            }
            else if (txtRestart && txtRestart->getGlobalBounds().contains(mPos)) {
                state = GameState::PLAYING;
                ballerine->reset();
                decor->reset();
                obstacles.clear();
                collectibles.clear();
                popups.clear();
                totalCollectiblesSpawned = 0;
                score = 0;
                collectiblePending = false;
                ui->reset();
                gameOverMusic.stop();
                if (musicOn) levelMusic.play();
                showSettingsMenu = false;
            }
            else if (txtQuit && txtQuit->getGlobalBounds().contains(mPos)) {
                // return to Menu2
                if (scenes) scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
            }
            else if (spriteSettings && spriteSettings->getGlobalBounds().contains(mPos)) {
                showSettingsMenu = false;
            }
        }
        else if (state == GameState::GAMEOVER_MENU) {
            if (sReplay && sReplay->getGlobalBounds().contains(mPos)) {
                state = GameState::PLAYING;
                ballerine->reset();
                decor->reset();
                obstacles.clear();
                collectibles.clear();
                popups.clear();
                totalCollectiblesSpawned = 0;
                score = 0;
                collectiblePending = false;
                ui->reset();
                gameOverMusic.stop();
                if (musicOn) levelMusic.play();
            }
            if (sQuit && sQuit->getGlobalBounds().contains(mPos)) {
                SaveManager::save(saveData, slotPath);
                scenes->setScene(std::make_unique<Menu2State>(scenes, saveData, slotIndex, slotPath));
            }

        }
        else if (spriteSettings->getGlobalBounds().contains(mPos)) {
            showSettingsMenu = true;
        }
    }

    if (state == GameState::PLAYING) {
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Left && ballerine->lane > 0) {
                ballerine->changeLane(ballerine->lane - 1);
            }
            if (kp->code == sf::Keyboard::Key::Right && ballerine->lane < 2) {
                ballerine->changeLane(ballerine->lane + 1);
            }
            if (kp->code == sf::Keyboard::Key::Space) {
                ballerine->jump();
            }
        }
    }
}

void GameplayState::update(float dt) {
    float gameDt = (showSettingsMenu || showSettingsPanel || state != GameState::PLAYING) ? 0.f : dt;

    if (state == GameState::PLAYING && gameDt > 0.f) {
        ballerine->update(gameDt);
        decor->update(gameDt, state);
        route->update(gameSpeed);

        if (scoreText) scoreText->setString(std::to_string((int)(decor->timer * 100) + score));

        if (spawnTimer.getElapsedTime().asSeconds() > 2.5f) {
            obstacles.push_back(std::make_unique<Obstacle>(oTex));
            if (totalCollectiblesSpawned < 20) {
                collectiblePending = true;
                collectibleTimer.restart();
                pendingLane = rand() % 3;
            }
            spawnTimer.restart();
        }

        if (collectiblePending && collectibleTimer.getElapsedTime().asSeconds() > 1.2f) {
            int randType = rand() % 4;
            collectibles.push_back(std::make_unique<Collectible>(*collectibleTextures[randType], pendingLane));
            totalCollectiblesSpawned++;
            collectiblePending = false;
        }

        for (auto it = obstacles.begin(); it != obstacles.end();) {
            if ((*it)->update(gameSpeed)) {
                it = obstacles.erase(it);
            }
            else {
                if ((*it)->lane == ballerine->lane && (*it)->progress > 0.65f && (*it)->progress < 0.85f && ballerine->y > GROUND_Y - 20.f) {
                    state = GameState::BALLERINE_FALLING;
                    SaveManager::save(saveData, slotPath); // AUTO SAVE ON DEATH
                    ballerine->state = PlayerState::DIE;
                    deathTimer.restart();
                    levelMusic.stop();
                    if (musicOn) gameOverMusic.play();
                    if (sfxOn && sfxDeath) sfxDeath->play();
                }
                ++it;
            }
        }

        for (auto it = collectibles.begin(); it != collectibles.end();) {
            if ((*it)->update(gameSpeed)) {
                it = collectibles.erase(it);
            }
            else {
                if ((*it)->lane == ballerine->lane && (*it)->progress > 0.7f && (*it)->progress < 0.9f) {
                    if (sfxOn && sfxCollec) sfxCollec->play();
                    score += 50;
                    const sf::Texture* collectedTex = &(*it)->sprite.getTexture();
                    if (collectedTex == &collTex1) {
                        popups.emplace_back(popupTex1);
                        saveData.coll1++;
                    }
                    else if (collectedTex == &collTex2) {
                        popups.emplace_back(popupTex2);
                        saveData.coll2++;
                    }
                    else if (collectedTex == &collTex3) {
                        popups.emplace_back(popupTex3);
                        saveData.coll3++;
                    }
                    else if (collectedTex == &collTex4) {
                        popups.emplace_back(popupTex4);
                        saveData.coll4++;
                    }

                    if (!popups.empty()) popups.back().sprite.setScale({0.15f, 0.15f});
                    it = collectibles.erase(it);
                } else ++it;
            }
        }

        float popupDuration = 2.f;
        float popupFadeDuration = 0.5f;
        for (auto it = popups.begin(); it != popups.end();) {
            it->timer += gameDt;
            it->sprite.move({0.f, 80.f * gameDt});
            if (it->timer > popupDuration - popupFadeDuration) {
                float ratio = 1.f - ((it->timer - (popupDuration - popupFadeDuration)) / popupFadeDuration);
                if (ratio < 0.f) ratio = 0.f;
                if (ratio > 1.f) ratio = 1.f;
                sf::Color c = it->sprite.getColor();
                c.a = static_cast<std::uint8_t>(255.f * ratio);
                it->sprite.setColor(c);
            }
            if (it->timer >= popupDuration) it = popups.erase(it); else ++it;
        }

        sf::Vector2f popupBasePos({WINDOW_WIDTH - 200.f, 270.f});
        float popupSpacing = 55.f;
        for (int i = 0; i < (int)popups.size(); ++i) {
            float offsetDown = popups[i].timer * 100.f;
            popups[i].sprite.setPosition({popupBasePos.x, popupBasePos.y + (i * popupSpacing) + offsetDown});
        }
    }
    else if (state == GameState::BALLERINE_FALLING) {
        ballerine->update(dt);
        if (deathTimer.getElapsedTime().asSeconds() > 2.0f) state = GameState::FADING_TO_GAMEOVER;
    }
    else if (state == GameState::FADING_TO_GAMEOVER) {
        ui->updateFade(dt, true);
        if (ui->blackScreen.getFillColor().a >= 254) state = GameState::GAMEOVER_MENU;
    }
}

void GameplayState::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(25, 25, 45));
    decor->draw(window);
    route->draw(window);
    for (auto& o : obstacles) window.draw(o->sprite);
    for (auto& c : collectibles) window.draw(c->sprite);
    window.draw(ballerine->sprite);
    if (spriteScoreIcon) window.draw(*spriteScoreIcon);
    if (scoreText) window.draw(*scoreText);
    if (spriteFullIventoryBack) window.draw(*spriteFullIventoryBack);
    for (auto& p : popups) window.draw(p.sprite);
    if (spriteSettings) window.draw(*spriteSettings);
    if (spriteFullIventory) window.draw(*spriteFullIventory);
    if (showSettingsMenu) {
        window.draw(settingslil);
        if (txtQuit) window.draw(*txtQuit);
        if (txtRestart) window.draw(*txtRestart);
        if (txtOpenSettings) window.draw(*txtOpenSettings);
    }
    if (state == GameState::FADING_TO_GAMEOVER || state == GameState::GAMEOVER_MENU) {
        if (state == GameState::GAMEOVER_MENU) ui->updateFade(0.f, false);
        window.draw(ui->blackScreen);
        if (state == GameState::GAMEOVER_MENU) {
            if (ui->bg) window.draw(*ui->bg);
            if (sReplay) window.draw(*sReplay);
            if (sQuit) window.draw(*sQuit);
            if (spriteLogo) window.draw(*spriteLogo);
        }
    }
    if (showSettingsPanel) {
        window.draw(bigPanel);
        if (pTitle) window.draw(*pTitle);
        if (pMusic) window.draw(*pMusic);
        if (pSfx) window.draw(*pSfx);
        if (pVol) window.draw(*pVol);
        if (pClose) window.draw(*pClose);
    }
}