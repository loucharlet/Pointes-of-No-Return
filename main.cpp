#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>
#include <memory>


// config
const float WINDOW_WIDTH = 1000.f;
const float WINDOW_HEIGHT = 800.f;
const float HORIZON_Y = 220.f;
const float GROUND_Y = 750.f;
const float LANES_END_X[] = {200.f, 500.f, 800.f};
const float LANES_START_X[] = {500.f, 500.f, 500.f};
const float PLAYER_LANES_END_X[] = {300.f, 500.f, 700.f};
const float gameSpeed = 0.005f;

enum class GameState { PLAYING, BALLERINE_FALLING, FADING_TO_GAMEOVER, GAMEOVER_MENU, LEVEL_COMPLETE };
enum class PlayerState { RUN, JUMP, MOVE, DIE };

class SideBuilding {
public:
    sf::Sprite sprite;
    bool isRight;
    float progress = 0.f;

    SideBuilding(const sf::Texture& tex, bool rightSide, float startProgress = 0.f)
        : sprite(tex), isRight(rightSide), progress(startProgress)
    {
        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin({b.size.x / 2.f, b.size.y});
    }

    bool update(float speed) {
        progress += speed * 0.3f;

        // TA FORMULE EXACTE (0.5f comme avant)
        float s = 0.1f + (progress * 0.5f);

        float flip = isRight ? -1.f : 1.f;
        sprite.setScale({flip * s, s});

        // TES POSITIONS EXACTES (150 / 500 / 600)
        float posY = (HORIZON_Y + 150.f) + (600.f * progress);
        float offsetX = 150.f + (progress * 500.f);
        float posX = isRight ? (500.f + offsetX) : (500.f - offsetX);

        sprite.setPosition({posX, posY});

        // Disparition lointaine pour pas que ça "pop"
        return progress > 1.5f || posY > WINDOW_HEIGHT + 600.f;
    }
};


struct SpecialEvent {
    float triggerP;
    const sf::Texture* tex;
    bool isRight;
    bool done = false;
};

class Decor {
public:
    sf::Sprite sky;
    sf::Sprite opera;
    std::vector<const sf::Texture*> buildingTextures;
    std::vector<std::unique_ptr<SideBuilding>> buildings;
    sf::Clock spawnClock;
    float timer = 0.f;
    const float targetTime = 90.f;
    bool isSpecialEvent = false;
    std::vector<SpecialEvent> specialTimeline;
    sf::Clock clockL, clockR;
    float nextDelayL = 2.f;
    float nextDelayR = 4.f;

    Decor(const sf::Texture& tSky, const sf::Texture& tOpera,
          const std::vector<const sf::Texture*>& tBuildings,
          const sf::Texture& tArc, const sf::Texture& tGalerie,
          const sf::Texture& tNotreDame, const sf::Texture& tMoulin)
        : sky(tSky), opera(tOpera), buildingTextures(tBuildings)
    {
        specialTimeline.push_back({0.15f, &tArc, false});
        specialTimeline.push_back({0.35f, &tGalerie, false});
        specialTimeline.push_back({0.60f, &tNotreDame, true});
        specialTimeline.push_back({0.85f, &tMoulin, true});

        sky.setPosition({0.f, 0.f});
        sf::FloatRect bOpera = opera.getLocalBounds();
        opera.setOrigin({bOpera.size.x / 2.f, bOpera.size.y / 2.f});
        opera.setPosition({500.f, HORIZON_Y + 50.f});
        opera.setScale({0.1f, 0.1f});
        spawnInitialBuildings(); // Elle est appelée ici...
    }

    // --- LA FONCTION QUI MANQUAIT ---
    void spawnInitialBuildings() {
        buildings.clear();
        for (int i = 2; i >= 0; i--) {
            float p = (float)i * 0.33f;
            const sf::Texture& texL = *buildingTextures[rand() % buildingTextures.size()];
            const sf::Texture& texR = *buildingTextures[rand() % buildingTextures.size()];
            buildings.push_back(std::make_unique<SideBuilding>(texL, false, p));
            buildings.push_back(std::make_unique<SideBuilding>(texR, true, p));
        }
    }

    void update(float dt, GameState state) {
    if (state == GameState::PLAYING) {
        timer += dt;
        if (timer > targetTime) timer = targetTime;

        float pLevel = timer / targetTime;

        // 1. GESTION DE L'OPÉRA (Grossit jusqu'à 3x sa taille)
        // On part de 0.1f et on ajoute 0.3f pour arriver à 0.4f (qui est 4x 0.1)
        float opS = 0.1f + (pLevel * 0.3f);
        opera.setScale({opS, opS});
        opera.setPosition({500.f, (HORIZON_Y + 35.f) - (pLevel * 200.f)});

        // 2. TIMELINE DES MONUMENTS
        SpecialEvent* currentEv = nullptr;
        bool monumentIncomingL = false;
        bool monumentIncomingR = false;

        for (auto& ev : specialTimeline) {
            if (!ev.done) {
                // On check si un monument arrive bientôt (3% de progression avant)
                // pour bloquer UNIQUEMENT le spawn normal de ce côté là
                if (pLevel >= (ev.triggerP - 0.03f)) {
                    if (ev.isRight) monumentIncomingR = true;
                    else monumentIncomingL = true;
                }
                // Si on a atteint ou dépassé l'heure du trigger
                if (pLevel >= ev.triggerP) {
                    currentEv = &ev;
                }
                break; // On gère un seul monument à la fois pour pas saturer
            }
        }

        // 3. SPAWN DU MONUMENT (Si prêt)
        if (currentEv) {
            sf::Clock& targetClock = currentEv->isRight ? clockR : clockL;
            // On attend que la lane soit vide depuis 8s pour le monument
            if (targetClock.getElapsedTime().asSeconds() > 8.0f) {
                buildings.push_back(std::make_unique<SideBuilding>(*currentEv->tex, currentEv->isRight, 0.f));
                currentEv->done = true;
                targetClock.restart(); // On reset l'horloge du côté concerné
            }
        }

        // 4. SPAWN NORMAL GAUCHE (Indépendant)
        // On spawn si : pas de monument à gauche ET délai de 3.5s passé
        if (!monumentIncomingL && clockL.getElapsedTime().asSeconds() > 3.5f) {
            const sf::Texture& tex = *buildingTextures[rand() % buildingTextures.size()];
            buildings.push_back(std::make_unique<SideBuilding>(tex, false, 0.f));
            clockL.restart();
        }

        // 5. SPAWN NORMAL DROIT (Indépendant)
        // On spawn si : pas de monument à droite ET délai de 3.8s passé
        if (!monumentIncomingR && clockR.getElapsedTime().asSeconds() > 3.8f) {
            const sf::Texture& tex = *buildingTextures[rand() % buildingTextures.size()];
            buildings.push_back(std::make_unique<SideBuilding>(tex, true, 0.f));
            clockR.restart();
        }

        // 6. UPDATE DE TOUS LES BÂTIMENTS (Normaux + Spéciaux)
        for (auto it = buildings.begin(); it != buildings.end(); ) {
            // Utilise ta formule stable (s = 0.1 + progress * 0.5)
            if ((*it)->update(gameSpeed)) {
                it = buildings.erase(it);
            } else {
                ++it;
            }
        }
    }
}

    void reset() {
        timer = 0.f; isSpecialEvent = false;
        for (auto& ev : specialTimeline) ev.done = false;
        opera.setScale({0.1f, 0.1f}); sky.setScale({1.f, 1.f}); sky.setOrigin({0.f, 0.f});
        spawnInitialBuildings(); // ... et ici
        spawnClock.restart();
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sky); window.draw(opera);
        for (auto it = buildings.rbegin(); it != buildings.rend(); ++it) window.draw((*it)->sprite);
    }
};

// --- CLASSE ROUTE ---

class Road {
public:
    sf::VertexArray va;
    sf::Texture& texture;
    float offset = 0.f;

    Road(sf::Texture& tex) : texture(tex), va(sf::PrimitiveType::Triangles) {
        texture.setRepeated(true);
    }

    void update(float speed) {
        offset += speed * 0.7f;
        if (offset > 1.0f) offset -= 1.0f;
        va.clear();
        const int precision = 40;
        float startY = HORIZON_Y + 100.f;
        float endY = WINDOW_HEIGHT;
        float h = endY - startY;

        for (int i = 0; i < precision; i++) {
            float p1 = (float)i / precision;
            float p2 = (float)(i + 1) / precision;
            float w1 = 0.15f + (p1 * 0.85f);
            float w2 = 0.15f + (p2 * 0.85f);
            float y1 = startY + p1 * h;
            float y2 = startY + p2 * h;
            float x1_l = 500.f - (w1 * 500.f);
            float x1_r = 500.f + (w1 * 500.f);
            float x2_l = 500.f - (w2 * 500.f);
            float x2_r = 500.f + (w2 * 500.f);
            float tx = (float)texture.getSize().x;
            float ty = (float)texture.getSize().y;
            float v1 = (p1 - offset) * ty * 2.0f;
            float v2 = (p2 - offset) * ty * 2.0f;
            sf::Color white = sf::Color::White;
            va.append(sf::Vertex({x1_l, y1}, white, {0.f, v1}));
            va.append(sf::Vertex({x1_r, y1}, white, {tx, v1}));
            va.append(sf::Vertex({x2_l, y2}, white, {0.f, v2}));
            va.append(sf::Vertex({x1_r, y1}, white, {tx, v1}));
            va.append(sf::Vertex({x2_r, y2}, white, {tx, v2}));
            va.append(sf::Vertex({x2_l, y2}, white, {0.f, v2}));
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::RenderStates states;
        states.texture = &texture;
        window.draw(va, states);
    }
};

// --- CLASSE OBSTACLE ---

class Obstacle {
public:
    sf::Sprite sprite;
    int lane;
    float progress = 0.f;
    Obstacle(const sf::Texture& tex) : sprite(tex), lane(rand() % 3) {}

    bool update(float speed) {
        progress += speed * 1.2f;
        float s = 0.01f + (progress * 0.09f);
        sprite.setScale({s, s});
        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin({b.size.x / 2.f, b.size.y});
        float posX = LANES_START_X[lane] + (LANES_END_X[lane] - LANES_START_X[lane]) * progress;
        float posY = (HORIZON_Y + 100.f) + (650.f * progress);
        sprite.setPosition({posX, posY});
        return progress > 1.1f;
    }
};

// --- CLASSE PLAYER ---

class Player {
public:
    sf::Sprite sprite;
    int lane = 1;
    float y = GROUND_Y;
    float vY = 0.f;
    PlayerState state = PlayerState::RUN;
    float jumpDelay = 0.f;
    int frameNum = 0;
    float animTimer = 0.f;
    float lastDir = 1.0f;

    std::vector<sf::IntRect> runFrames = {{{0,3},{170,210}}, {{185,3},{170,210}}, {{380,0},{170,210}}, {{550,0},{170,210}}, {{730,0},{170,210}}, {{950,0},{170,210}}};
    std::vector<sf::IntRect> jumpFrames = {{{0,590},{170,250}}, {{170,590},{170,250}}, {{340,590},{170,250}}, {{550,590},{170,250}}, {{710,590},{170,250}}, {{890,590},{170,250}}};
    std::vector<sf::IntRect> moveFrames = {{{0,410},{170,175}}, {{190,410},{170,175}}, {{360,410},{170,178}}, {{560,410},{170,177}}, {{730,410},{170,175}}, {{950,410},{170,175}}};
    std::vector<sf::IntRect> dieFrames = {{{0,235},{170,175}}, {{179,230},{170,175}}, {{380,230},{170,175}}, {{560,230},{170,175}}, {{740,230},{170,175}}, {{935,230},{170,175}}};

    Player(const sf::Texture& tex) : sprite(tex) { updateSpriteRect(runFrames[0]); }

    void updateSpriteRect(sf::IntRect rect) {
        sprite.setTextureRect(rect);
        sprite.setOrigin({(float)rect.size.x / 2.f, (float)rect.size.y});
    }

    void jump() {
        if (state == PlayerState::RUN) {
            state = PlayerState::JUMP;
            vY = -14.f;
            jumpDelay = 0.2f;
            frameNum = 0;
        }
    }

    void changeLane(int newLane) {
        if (state != PlayerState::DIE) {
            if (newLane < lane) lastDir = -1.0f; else if (newLane > lane) lastDir = 1.0f;
            lane = newLane;
            if (state == PlayerState::RUN) { state = PlayerState::MOVE; frameNum = 0; }
        }
    }

    void reset() { lane = 1; y = GROUND_Y; vY = 0.f; state = PlayerState::RUN; frameNum = 0; animTimer = 0.f; lastDir = 1.0f; }

    void update(float dt) {
        animTimer += dt;
        sprite.setScale({lastDir * 0.8f, 0.8f});
        float targetX = PLAYER_LANES_END_X[lane];
        float currX = sprite.getPosition().x;
        sprite.setPosition({currX + (targetX - currX) * 0.15f, y});
        if (jumpDelay > 0.f) { jumpDelay -= dt; updateSpriteRect(jumpFrames[0]); return; }
        std::vector<sf::IntRect>* currentList = &runFrames;
        float speed = 0.12f;
        if (state == PlayerState::JUMP) {
            currentList = &jumpFrames;
            speed = 0.3f;
            vY += 0.32f;
            y += vY;
            if (y >= GROUND_Y) { y = GROUND_Y; vY = 0.f; state = PlayerState::RUN; }
        }
        else if (state == PlayerState::MOVE) { currentList = &moveFrames; speed = 0.08f; }
        else if (state == PlayerState::DIE) { currentList = &dieFrames; speed = 0.2f; }
        if (animTimer > speed) {
            frameNum++;
            if (frameNum >= (int)currentList->size()) {
                if (state == PlayerState::DIE) frameNum = (int)currentList->size() - 1;
                else if (state == PlayerState::MOVE) { state = PlayerState::RUN; frameNum = 0; }
                else frameNum = 0;
            }
            animTimer = 0.f;
        }
        if (frameNum < (int)currentList->size()) updateSpriteRect((*currentList)[frameNum]);
    }
};

// --- CLASSE GAMEOVER ---

class GameOverUI {
public:
    sf::Texture tex;
    std::unique_ptr<sf::Sprite> bg;
    sf::RectangleShape blackScreen;
    float alpha = 0.f;
    GameOverUI() {
        blackScreen.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
        blackScreen.setFillColor(sf::Color(0, 0, 0, 0));
    }
    bool load(const std::string& path) {
        if (!tex.loadFromFile(path)) return false;
        bg = std::make_unique<sf::Sprite>(tex);
        bg->setColor(sf::Color(255, 255, 255, 0));
        return true;
    }
    void updateFade(float dt, bool toBlack) {
        alpha += 150.f * dt; if (alpha > 255.f) alpha = 255.f;
        uint8_t a = static_cast<uint8_t>(alpha);
        if (toBlack) blackScreen.setFillColor(sf::Color(0, 0, 0, a));
        else if (bg) bg->setColor(sf::Color(255, 255, 255, a));
    }
    void reset() { alpha = 0.f; blackScreen.setFillColor(sf::Color(0, 0, 0, 0)); if (bg) bg->setColor(sf::Color(255, 255, 255, 0)); }
};

// --- MAIN ---

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Pointes of No Return");
    window.setFramerateLimit(60);

    // --- CHARGEMENT TEXTURES ---
    sf::Texture pTex, oTex, texBtnReplay, texBtnQuit, skyTex, operaTex, roadTex, bTex1, bTex2, bTex3, tArc, tGalerie, tNotreDame, tMoulin, texSettings, texScoreIcon;
    if (!pTex.loadFromFile("./assets/player_lvl1_pink.png")) return -1;
    if (!oTex.loadFromFile("./assets/obstacle.png")) return -1;
    if (!texBtnReplay.loadFromFile("./assets/REPLAY.png")) return -1;
    if (!texBtnQuit.loadFromFile("./assets/QUIT.png")) return -1;
    if (!skyTex.loadFromFile("./assets/bg_sky.png")) return -1;
    if (!operaTex.loadFromFile("./assets/opéra.png")) return -1;
    if (!roadTex.loadFromFile("./assets/roof_ground.png")) return -1;
    if (!bTex1.loadFromFile("./assets/building1.png")) return -1;
    if (!bTex2.loadFromFile("./assets/building2.png")) return -1;
    if (!bTex3.loadFromFile("./assets/building3.png")) return -1;
    if (!tArc.loadFromFile("./assets/arc_de_triomphe.png") ||
        !tGalerie.loadFromFile("./assets/galeries_lafayette.png") ||
        !tNotreDame.loadFromFile("./assets/notre_dame.png") ||
        !tMoulin.loadFromFile("./assets/moulin_rouge.png")) return -1;
    if (!texSettings.loadFromFile("./assets/settings_icon.png")) return -1;
    if (!texScoreIcon.loadFromFile("./assets/score_icon.png")) return -1;

    // --- CHARGEMENT MUSIQUES (NOUVEAU) ---
    sf::Music levelMusic;
    if (!levelMusic.openFromFile("./assets/level_theme.ogg")) return -1;
    levelMusic.setLooping(true); // Relance à l'infini

    sf::Music gameOverMusic;
    if (!gameOverMusic.openFromFile("./assets/gameover_theme.ogg")) return -1;
    gameOverMusic.setLooping(true);

    bool musicEnabled = true;
    levelMusic.play();

    // --- INIT OBJETS ---
    std::vector<const sf::Texture*> texVariations = {&bTex1, &bTex2, &bTex3};
    Decor decor(skyTex, operaTex, texVariations, tArc, tGalerie, tNotreDame, tMoulin);
    Player ballerine(pTex);
    Road route(roadTex);
    GameOverUI ui;
    ui.load("./assets/gameover.png");

    // --- UI SETUP ---
    sf::FloatRect btnReplay({260.f, 380.f}, {200.f, 80.f});
    sf::FloatRect btnQuit({560.f, 380.f}, {200.f, 80.f});
    sf::Sprite spriteReplay(texBtnReplay); spriteReplay.setPosition(btnReplay.position);
    sf::Sprite spriteQuit(texBtnQuit); spriteQuit.setPosition(btnQuit.position);

    sf::Sprite spriteSettings(texSettings);
    spriteSettings.setScale({0.35f, 0.35f});
    spriteSettings.setPosition({WINDOW_WIDTH - 77.f, 35.f});

    sf::Sprite spriteScoreIcon(texScoreIcon);
    spriteScoreIcon.setScale({0.57f, 0.57f});
    spriteScoreIcon.setPosition({17.f, 20.f});

    sf::Font font;
    font.openFromFile("./assets/police_futura.ttf");
    sf::Text scoreText(font, "0", 20);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setOutlineThickness(3.f);
    scoreText.setPosition({150.f, 44.f});

    // --- MENU DROPDOWN ---
    bool showSettingsMenu = false;
    sf::RectangleShape dropdownBg({180.f, 150.f});
    dropdownBg.setFillColor(sf::Color(50, 50, 50, 220));
    dropdownBg.setOutlineThickness(2.f);
    dropdownBg.setOutlineColor(sf::Color::White);
    dropdownBg.setPosition({WINDOW_WIDTH - 200.f, 100.f});

    sf::Text txtRestart(font, "RESTART", 20);
    sf::Text txtSettings(font, "SETTINGS", 20);
    sf::Text txtQuitMenu(font, "QUIT", 20);
    txtRestart.setPosition({WINDOW_WIDTH - 185.f, 115.f});
    txtSettings.setPosition({WINDOW_WIDTH - 185.f, 155.f});
    txtQuitMenu.setPosition({WINDOW_WIDTH - 185.f, 195.f});

    // --- PANNEAU SETTINGS ---
    bool showSettingsPanel = false;
    sf::RectangleShape settingsPanel({400.f, 300.f});
    settingsPanel.setFillColor(sf::Color(40, 40, 40, 245));
    settingsPanel.setOutlineThickness(3.f);
    settingsPanel.setOutlineColor(sf::Color::White);
    settingsPanel.setOrigin({200.f, 150.f});
    settingsPanel.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});

    sf::Text settingsTitle(font, "--- REGLAGES ---", 30);
    settingsTitle.setOrigin({settingsTitle.getLocalBounds().size.x / 2.f, 0.f});
    settingsTitle.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 120.f});

    sf::Text txtClose(font, "X", 25);
    txtClose.setFillColor(sf::Color::Red);
    txtClose.setPosition({WINDOW_WIDTH / 2.f + 160.f, WINDOW_HEIGHT / 2.f - 140.f});

    sf::Text txtMusic(font, "MUSIQUE : ON", 20);
    txtMusic.setPosition({WINDOW_WIDTH / 2.f - 100.f, WINDOW_HEIGHT / 2.f - 30.f});

    sf::Text txtDiff(font, "DIFFICULTE : NORMALE", 20);
    txtDiff.setPosition({WINDOW_WIDTH / 2.f - 100.f, WINDOW_HEIGHT / 2.f + 20.f});

    bool musicOn = true;

    // --- GAME LOOP VARS ---
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    sf::Clock animClock, spawnTimer, deathTimer;
    GameState state = GameState::PLAYING;
    int score = 0;

    while (window.isOpen()) {
        float dt = animClock.restart().asSeconds();
        // Pause si un menu est ouvert
        float gameDt = (showSettingsMenu || showSettingsPanel || state != GameState::PLAYING) ? 0.f : dt;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mPos = window.mapPixelToCoords(mbp->position);

                // PRIORITÉ 1 : Si le gros panneau central est ouvert
                if (showSettingsPanel) {
                    // Bouton Fermer (X)
                    if (txtClose.getGlobalBounds().contains(mPos)) {
                        showSettingsPanel = false;
                    }
                    // Bouton Musique ON/OFF
                    else if (txtMusic.getGlobalBounds().contains(mPos)) {
                        musicEnabled = !musicEnabled;
                        txtMusic.setString(musicEnabled ? "MUSIQUE : ON" : "MUSIQUE : OFF");
                        txtMusic.setFillColor(musicEnabled ? sf::Color::Green : sf::Color::Red);

                        if (!musicEnabled) {
                            levelMusic.pause();
                            gameOverMusic.pause();
                        } else {
                            if (state == GameState::PLAYING) levelMusic.play();
                            else if (state == GameState::GAMEOVER_MENU) gameOverMusic.play();
                        }
                    }
                    // On "return" ou on skip le reste pour pas cliquer derrière le panneau
                }

                // PRIORITÉ 2 : Si le petit menu dropdown est ouvert
                else if (showSettingsMenu) {
                    if (txtSettings.getGlobalBounds().contains(mPos)) {
                        showSettingsPanel = true;
                        showSettingsMenu = false;
                    }
                    else if (txtRestart.getGlobalBounds().contains(mPos)) {
                        gameOverMusic.stop();
                        if (musicEnabled) levelMusic.play();
                        state = GameState::PLAYING; ballerine.reset(); decor.reset();
                        obstacles.clear(); ui.reset(); showSettingsMenu = false;
                    }
                    else if (txtQuitMenu.getGlobalBounds().contains(mPos)) {
                        window.close();
                    }
                    // Si on clique ailleurs, on ferme le menu
                    else if (!spriteSettings.getGlobalBounds().contains(mPos)) {
                        showSettingsMenu = false;
                    }
                }

                // PRIORITÉ 3 : Les clics sur le jeu (Engrenage, Replay Game Over)
                else {
                    // Clic sur l'icône engrenage
                    if (spriteSettings.getGlobalBounds().contains(mPos)) {
                        showSettingsMenu = true;
                    }

                    // Clic sur REPLAY dans l'écran Game Over
                    if (state == GameState::GAMEOVER_MENU && btnReplay.contains(mPos)) {
                        gameOverMusic.stop();
                        if (musicEnabled) levelMusic.play();
                        state = GameState::PLAYING; ballerine.reset(); decor.reset();
                        obstacles.clear(); ui.reset();
                    }
                }
            }

            // Touches clavier (uniquement si pas en pause)
            if (gameDt > 0.f && state == GameState::PLAYING) {
                if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                    if (kp->code == sf::Keyboard::Key::Left && ballerine.lane > 0) ballerine.changeLane(ballerine.lane - 1);
                    if (kp->code == sf::Keyboard::Key::Right && ballerine.lane < 2) ballerine.changeLane(ballerine.lane + 1);
                    if (kp->code == sf::Keyboard::Key::Space) ballerine.jump();
                }
            }
        }

        // --- UPDATE ---
        ballerine.update(gameDt);
        if (state == GameState::PLAYING && gameDt > 0.f) {
            decor.update(gameDt, state);
            route.update(gameSpeed);
            score = static_cast<int>(decor.timer * 100);
            scoreText.setString(std::to_string(score));

            if (spawnTimer.getElapsedTime().asSeconds() > 1.8f) {
                obstacles.push_back(std::make_unique<Obstacle>(oTex));
                spawnTimer.restart();
            }

            for (auto it = obstacles.begin(); it != obstacles.end();) {
                if ((*it)->update(gameSpeed)) it = obstacles.erase(it);
                else {
                    if ((*it)->lane == ballerine.lane && (*it)->progress > 0.60f && (*it)->progress < 0.85f && ballerine.y > GROUND_Y - 10.f) {
                        levelMusic.stop();
                        if (musicEnabled) gameOverMusic.play();
                        ballerine.state = PlayerState::DIE; deathTimer.restart(); state = GameState::BALLERINE_FALLING; break;
                    }
                    ++it;
                }
            }
        }

        if (state == GameState::BALLERINE_FALLING) {
            ballerine.update(dt); // on update l'anim de mort même sans gameDt
            if (deathTimer.getElapsedTime().asSeconds() > 2.0f) { ui.alpha = 0; state = GameState::FADING_TO_GAMEOVER; }
        }
        else if (state == GameState::FADING_TO_GAMEOVER) {
            if (ui.blackScreen.getFillColor().a < 255) ui.updateFade(dt, true);
            else { ui.updateFade(dt, false); if (ui.bg && ui.bg->getColor().a == 255) state = GameState::GAMEOVER_MENU; }
        }

        // --- DRAW ---
        window.clear(sf::Color(25, 25, 45));
        decor.draw(window);
        route.draw(window);
        for (auto& o : obstacles) window.draw(o->sprite);
        window.draw(ballerine.sprite);

        if (state == GameState::FADING_TO_GAMEOVER || state == GameState::GAMEOVER_MENU) {
            window.draw(ui.blackScreen);
            if (ui.bg) window.draw(*ui.bg);
            if (state == GameState::GAMEOVER_MENU) { window.draw(spriteReplay); window.draw(spriteQuit); }
        }

        window.draw(spriteScoreIcon);
        window.draw(scoreText);
        window.draw(spriteSettings);

        if (showSettingsMenu) {
            window.draw(dropdownBg);
            window.draw(txtRestart); window.draw(txtSettings); window.draw(txtQuitMenu);
        }

        if (showSettingsPanel) {
            window.draw(settingsPanel);
            window.draw(settingsTitle);
            window.draw(txtClose);
            window.draw(txtMusic);
            window.draw(txtDiff);

            sf::Text txtHelp(font, "FLECHES : BOUGER | ESPACE : SAUTER", 15);
            txtHelp.setFillColor(sf::Color(150, 150, 150));
            txtHelp.setPosition({WINDOW_WIDTH / 2.f - 140.f, WINDOW_HEIGHT / 2.f + 100.f});
            window.draw(txtHelp);
        }

        window.display();
    }
    return 0;
}