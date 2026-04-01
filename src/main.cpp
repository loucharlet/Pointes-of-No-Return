#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <vector>
#include <memory>
#include <iostream>

#include "Constants.h"
#include "Player.h"
#include "Decor.h"
#include "Road.h"
#include "Obstacle.h"
#include "Collectible.h"
#include "SideBuilding.h"
#include "GameOverUI.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Pointes of No Return");
    window.setFramerateLimit(60);

    // textures
    sf::Texture pTex, oTex, texBtnReplay, texBtnQuit, skyTex, operaTex, roadTex, bTex1, bTex2, bTex3, tArc, tGalerie, tNotreDame, tMoulin, texSettings, texScoreIcon, texSettingsbg, texClouds, tLogo;
    pTex.loadFromFile("./assets/img/player_lvl1_pink.png");
    oTex.loadFromFile("./assets/img/obstacle.png");
    texBtnReplay.loadFromFile("./assets/img/REPLAY.png");
    texBtnQuit.loadFromFile("./assets/img/QUIT.png");
    skyTex.loadFromFile("./assets/img/bg_sky.png");
    texClouds.loadFromFile("./assets/img/clouds.png");
    operaTex.loadFromFile("./assets/img/opéra.png");
    roadTex.loadFromFile("./assets/img/roof_ground.png");
    bTex1.loadFromFile("./assets/img/building1.png");
    bTex2.loadFromFile("./assets/img/building2.png");
    bTex3.loadFromFile("./assets/img/building3.png");
    tArc.loadFromFile("./assets/img/arc_de_triomphe.png");
    tGalerie.loadFromFile("./assets/img/galeries_lafayette.png");
    tNotreDame.loadFromFile("./assets/img/notre_dame.png");
    tMoulin.loadFromFile("./assets/img/moulin_rouge.png");
    texSettings.loadFromFile("./assets/img/settings_icon.png");
    texScoreIcon.loadFromFile("./assets/img/score_icon.png");
    texSettingsbg.loadFromFile("./assets/img/settingsbg.png");
    tLogo.loadFromFile("./assets/img/logo.png");



    sf::Texture collTex1, collTex2, collTex3, collTex4;
    collTex1.loadFromFile("./assets/img/coll1.png");
    collTex2.loadFromFile("./assets/img/coll2.png");
    collTex3.loadFromFile("./assets/img/coll3.png");
    collTex4.loadFromFile("./assets/img/coll4.png");

    // audio
    sf::Music levelMusic, gameOverMusic;
    levelMusic.openFromFile("./assets/audio/level_theme.ogg");
    levelMusic.setLooping(true);
    gameOverMusic.openFromFile("./assets/audio/gameover_theme.ogg");
    gameOverMusic.setLooping(true);
    levelMusic.play();

    sf::SoundBuffer bufCollec, bufDeath, bufStart;
    bufCollec.loadFromFile("./assets/audio/collectibles.ogg");
    bufDeath.loadFromFile("./assets/audio/death.ogg");
    bufStart.loadFromFile("./assets/audio/gamestart.ogg");

    sf::Sound sfxCollec(bufCollec), sfxDeath(bufDeath), sfxStart(bufStart);

    // init objets
    std::vector<const sf::Texture*> texVariations = {&bTex1, &bTex2, &bTex3};
    Decor decor(skyTex, operaTex, texVariations, tArc, tGalerie, tNotreDame, tMoulin, texClouds);
    Player ballerine(pTex);
    Road route(roadTex);
    GameOverUI ui;
    ui.load("./assets/img/gameover.png");

    sf::Font font;
    font.openFromFile("./assets/police_futura.ttf");

    // UI
    sf::Text scoreText(font, "0", 25);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setOutlineThickness(2.f);
    scoreText.setPosition({150.f, 44.f});
    sf::Sprite spriteScoreIcon(texScoreIcon);
    spriteScoreIcon.setScale({0.57f, 0.57f});
    spriteScoreIcon.setPosition({17.f, 20.f});
    sf::Sprite spriteSettings(texSettings);
    spriteSettings.setScale({0.35f, 0.35f});
    spriteSettings.setPosition({WINDOW_WIDTH - 80.f, 30.f});

    sf::Sprite spriteLogo(tLogo);
    spriteLogo.setScale({0.075f, 0.075f});
    spriteLogo.setPosition({WINDOW_WIDTH - 240.f, 610.f});

    // Menus
    sf::RectangleShape settingslil({160.f, 120.f});
    settingslil.setFillColor(sf::Color::Black);
    settingslil.setPosition({WINDOW_WIDTH - 180.f, 85.f});

    sf::Text txtQuit(font, "Quit", 22), txtRestart(font, "Restart", 22), txtOpenSettings(font, "+ settings", 22);
    txtQuit.setPosition({WINDOW_WIDTH - 170.f, 95.f});
    txtRestart.setPosition({WINDOW_WIDTH - 170.f, 130.f});
    txtOpenSettings.setPosition({WINDOW_WIDTH - 170.f, 165.f});

    sf::RectangleShape bigPanel({600.f, 500.f});
    bigPanel.setTexture(&texSettingsbg);
    bigPanel.setOutlineThickness(5.f);
    bigPanel.setOutlineColor(sf::Color::Black);
    bigPanel.setOrigin({300.f, 250.f});
    bigPanel.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 60.f});

    sf::Text pTitle(font, "SETTINGS", 45);
    pTitle.setPosition({WINDOW_WIDTH/2.f - 100.f, 175.f});
    sf::Text pMusic(font, "Music: ON", 30); pMusic.setPosition({WINDOW_WIDTH/2.f - 100.f, 280.f});
    sf::Text pSfx(font, "SFX: ON", 30); pSfx.setPosition({WINDOW_WIDTH/2.f - 100.f, 340.f});
    sf::Text pVol(font, "Volume: 100", 30); pVol.setPosition({WINDOW_WIDTH/2.f - 100.f, 400.f});
    sf::Text pClose(font, "X", 25); pClose.setPosition({WINDOW_WIDTH/2.f + 270.f, 100.f});
    pClose.setFillColor(sf::Color::Red);
    pTitle.setOutlineColor(sf::Color::Black);
    pTitle.setOutlineThickness(3.f);

    pMusic.setOutlineColor(sf::Color::Black);
    pMusic.setOutlineThickness(3.f);

    pSfx.setOutlineColor(sf::Color::Black);
    pSfx.setOutlineThickness(3.f);

    pVol.setOutlineColor(sf::Color::Black);
    pVol.setOutlineThickness(3.f);

    sf::Sprite sReplay(texBtnReplay), sQuit(texBtnQuit), sLogo(tLogo);
    sReplay.setPosition({250.f, 400.f});
    sQuit.setPosition({550.f, 400.f});


    // Logic vars
    std::vector<sf::Texture*> collectibleTextures = {&collTex1, &collTex2, &collTex3, &collTex4};
    std::vector<std::unique_ptr<Collectible>> collectibles;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    sf::Clock animClock, spawnTimer, deathTimer, collectibleTimer;
    GameState state = GameState::PLAYING;
    bool showSettingsMenu = false;
    bool showSettingsPanel = false;
    float volume = 100.f;
    bool musicOn = true;
    bool sfxOn = true;
    int totalCollectiblesSpawned = 0;
    int score = 0;
    //coll+obs logic timer
    bool collectiblePending = false; // pour savoir si un item attend de spawner
    int pendingLane = 0; // la lane où l'item doit aller

    while (window.isOpen()) {
        float dt = animClock.restart().asSeconds();
        float gameDt = (showSettingsMenu || showSettingsPanel || state != GameState::PLAYING) ? 0.f : dt;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

                if (showSettingsPanel) {
                    if (pClose.getGlobalBounds().contains(mPos)) showSettingsPanel = false;
                    if (pMusic.getGlobalBounds().contains(mPos)) {
                        musicOn = !musicOn;
                        if (musicOn) levelMusic.play(); else levelMusic.pause();
                        pMusic.setString(musicOn ? "Music: ON" : "Music: OFF");
                    }
                    if (pVol.getGlobalBounds().contains(mPos)) {
                        volume = (volume >= 100.f) ? 0.f : volume + 20.f;
                        levelMusic.setVolume(volume);
                        pVol.setString("Volume: " + std::to_string((int)volume));
                    }
                    if (pSfx.getGlobalBounds().contains(mPos)) {
                        sfxOn = !sfxOn;
                        pSfx.setString(sfxOn ? "SFX: ON" : "SFX: OFF");
                    }
                }
                else if (showSettingsMenu) {
                    if (txtOpenSettings.getGlobalBounds().contains(mPos)) { showSettingsPanel = true; showSettingsMenu = false; }
                    else if (txtRestart.getGlobalBounds().contains(mPos)) { state = GameState::PLAYING; ballerine.reset(); decor.reset(); obstacles.clear(); collectibles.clear(); totalCollectiblesSpawned = 0; ui.reset(); showSettingsMenu = false; }
                    else if (txtQuit.getGlobalBounds().contains(mPos)) window.close();
                    else if (spriteSettings.getGlobalBounds().contains(mPos)) showSettingsMenu = false;
                }
                else if (state == GameState::GAMEOVER_MENU) {
                    if (sReplay.getGlobalBounds().contains(mPos)) { state = GameState::PLAYING; ballerine.reset(); decor.reset(); obstacles.clear(); collectibles.clear(); totalCollectiblesSpawned = 0; ui.reset(); gameOverMusic.stop(); if(musicOn) levelMusic.play(); }
                    if (sQuit.getGlobalBounds().contains(mPos)) window.close();
                }
                else if (spriteSettings.getGlobalBounds().contains(mPos)) {
                    showSettingsMenu = true;
                }
            }

            if (gameDt > 0.f && state == GameState::PLAYING) {
                if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                    if (kp->code == sf::Keyboard::Key::Left && ballerine.lane > 0) ballerine.changeLane(ballerine.lane - 1);
                    if (kp->code == sf::Keyboard::Key::Right && ballerine.lane < 2) ballerine.changeLane(ballerine.lane + 1);
                    if (kp->code == sf::Keyboard::Key::Space) {
                        ballerine.jump();
                    }
                }
            }
        }

        if (state == GameState::PLAYING && gameDt > 0.f) {
            ballerine.update(gameDt);
            decor.update(gameDt, state);
            route.update(gameSpeed);

            //score :Timer + pt bonus
            scoreText.setString(std::to_string((int)(decor.timer * 100) + score));

            // SPAWN OBSTACLES (chaque 2.5s)
            if (spawnTimer.getElapsedTime().asSeconds() > 2.5f) {
                obstacles.push_back(std::make_unique<Obstacle>(oTex));

                // collectibles qui arrive
                if (totalCollectiblesSpawned < 20) {
                    collectiblePending = true;
                    collectibleTimer.restart();
                    //random lane
                    pendingLane = rand() % 3;
                }
                spawnTimer.restart();
            }

            // SPAWN COLLECTIBLES 1.2s après l'obstacle
            if (collectiblePending && collectibleTimer.getElapsedTime().asSeconds() > 1.2f) {
                int randType = rand() % 4;
                collectibles.push_back(std::make_unique<Collectible>(*collectibleTextures[randType], pendingLane));
                totalCollectiblesSpawned++;
                collectiblePending = false;
            }

            //logic Obstacles
            for (auto it = obstacles.begin(); it != obstacles.end();) {
                if ((*it)->update(gameSpeed)) it = obstacles.erase(it);
                else {
                    if ((*it)->lane == ballerine.lane && (*it)->progress > 0.65f && (*it)->progress < 0.85f && ballerine.y > GROUND_Y - 20.f) {
                        state = GameState::BALLERINE_FALLING;
                        ballerine.state = PlayerState::DIE;
                        deathTimer.restart();
                        levelMusic.stop();
                        if (musicOn) gameOverMusic.play();
                        if (sfxOn) sfxDeath.play();
                    }
                    ++it;
                }
            }

            // logic Collectibles
            for (auto it = collectibles.begin(); it != collectibles.end();) {
                if ((*it)->update(gameSpeed)) it = collectibles.erase(it);
                else {
                    if ((*it)->lane == ballerine.lane && (*it)->progress > 0.7f && (*it)->progress < 0.9f) {
                        if (sfxOn) sfxCollec.play();
                        score += 50; //bonus 50 pts par item
                        it = collectibles.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
        else if (state == GameState::BALLERINE_FALLING) {
            ballerine.update(dt);
            if (deathTimer.getElapsedTime().asSeconds() > 2.0f) state = GameState::FADING_TO_GAMEOVER;
        }
        else if (state == GameState::FADING_TO_GAMEOVER) {
            ui.updateFade(dt, true);
            if (ui.blackScreen.getFillColor().a >= 254) state = GameState::GAMEOVER_MENU;
        }

        window.clear(sf::Color(25, 25, 45));
        decor.draw(window);
        route.draw(window);
        for (auto& o : obstacles) window.draw(o->sprite);
        for (auto& c : collectibles) window.draw(c->sprite);
        window.draw(ballerine.sprite);
        window.draw(spriteScoreIcon);
        window.draw(scoreText);
        window.draw(spriteSettings);
        window.draw(spriteLogo);

        if (showSettingsMenu) {
            window.draw(settingslil);
            window.draw(txtQuit);
            window.draw(txtRestart);
            window.draw(txtOpenSettings);
        }

        if (state == GameState::FADING_TO_GAMEOVER || state == GameState::GAMEOVER_MENU) {
            if (state == GameState::GAMEOVER_MENU) ui.updateFade(dt, false);
            window.draw(ui.blackScreen);
            if (state == GameState::GAMEOVER_MENU) {
                if (ui.bg) window.draw(*ui.bg);
                window.draw(sReplay);
                window.draw(sQuit);
            }
        }

        if (showSettingsPanel) {
            window.draw(bigPanel);
            window.draw(pTitle);
            window.draw(pMusic);
            window.draw(pSfx);
            window.draw(pVol);
            window.draw(pClose);
        }
        window.display();
    }
    return 0;
}