#include <SFML/Graphics.hpp>
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
const float gameSpeed = 0.005f;

enum class GameState { PLAYING, BALLERINE_FALLING, FADING_TO_GAMEOVER, GAMEOVER_MENU };
enum class PlayerState { RUN, JUMP, MOVE, DIE };

//classes

class SideBuilding {
public:
    sf::Sprite sprite;
    bool isRight;
    float progress = 0.f;

    // on ajoute startProgress ici
    SideBuilding(const sf::Texture& tex, bool rightSide, float startProgress = 0.f)
        : sprite(tex), isRight(rightSide), progress(startProgress)
    {
        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin({b.size.x / 2.f, b.size.y});
    }

    bool update(float speed) {
        progress += speed * 0.3f;
        float s = 0.1f + (progress * 0.6f);
        float flip = isRight ? 1.f : -1.f;
        sprite.setScale({flip * s, s});

        float posY = (HORIZON_Y + 150.f) + (600.f * progress);
        float offsetX = 150.f + (progress * 700.f);
        float posX = isRight ? (500.f + offsetX) : (500.f - offsetX);

        sprite.setPosition({posX, posY});
        return progress > 1.0f || posY > WINDOW_HEIGHT + 300.f;
    }
};

class Decor {
public:
    sf::Sprite sky;
    sf::Sprite opera;
    const sf::Texture& buildingTex;
    std::vector<std::unique_ptr<SideBuilding>> buildings;
    sf::Clock spawnClock;
    float timer = 0.f;
    const float targetTime = 120.f;

    void spawnInitialBuildings() {
        buildings.clear();
        // On passe à 3 paires au lieu de 5 pour aérer
        for (int i = 2; i >= 0; i--) {
            float p = (float)i * 0.33f; // mieux répartis sur 3 points
            buildings.push_back(std::make_unique<SideBuilding>(buildingTex, true, p));
            buildings.push_back(std::make_unique<SideBuilding>(buildingTex, false, p));
        }
    }

    Decor(const sf::Texture& tSky, const sf::Texture& tOpera, const sf::Texture& tBuilding)
        : sky(tSky), opera(tOpera), buildingTex(tBuilding)
    {
        sky.setPosition({0.f, 0.f});
        sf::FloatRect bOpera = opera.getLocalBounds();
        opera.setOrigin({bOpera.size.x / 2.f, bOpera.size.y / 2.f});
        opera.setPosition({500.f, HORIZON_Y + 50.f});
        opera.setScale({0.1f, 0.1f});

        spawnInitialBuildings(); // call au debut
    }

    void update(float dt, GameState state) {
        if (state == GameState::PLAYING) {
            timer += dt;
            if (timer > targetTime) timer = targetTime;
            float pOp = timer / targetTime;
            opera.setScale({0.1f + (pOp * 0.9f), 0.1f + (pOp * 0.9f)});
            opera.setPosition({500.f, (HORIZON_Y + 35.f) - (pOp * 200.f)});

            if (spawnClock.getElapsedTime().asSeconds() > 5.f) {
                // push_back normal, le draw s'occupe de le mettre au fond
                buildings.push_back(std::make_unique<SideBuilding>(buildingTex, true, 0.f));
                buildings.push_back(std::make_unique<SideBuilding>(buildingTex, false, 0.f));
                spawnClock.restart();
            }

            for (auto it = buildings.begin(); it != buildings.end(); ) {
                if ((*it)->update(gameSpeed)) it = buildings.erase(it);
                else ++it;
            }
        }
    }

    void reset() {
        timer = 0.f;
        opera.setScale({0.1f, 0.1f});
        opera.setPosition({500.f, HORIZON_Y + 50.f});
        spawnInitialBuildings(); // on les remet a chaque mort
        spawnClock.restart();
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sky);
        window.draw(opera);

        // Dessine de la fin de la liste (nouveaux/fond) vers le début (vieux/devant)
        for (auto it = buildings.rbegin(); it != buildings.rend(); ++it) {
            window.draw((*it)->sprite);
        }
    }
};

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

            // trapeze
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

class Obstacle {
public:
    sf::Sprite sprite;
    int lane;
    float progress = 0.f;
    Obstacle(const sf::Texture& tex) : sprite(tex), lane(rand() % 3) {}

    bool update(float speed) {
        // On multiplie la vitesse par 1.3 pour qu'ils aillent un chouia plus vite
        progress += speed * 1.2f;

        // Taille réduite : 0.08f au lieu de 0.11f pour que ça grossisse moins à la fin
        float s = 0.01f + (progress * 0.09f);
        sprite.setScale({s, s});

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin({b.size.x / 2.f, b.size.y});

        float posX = LANES_START_X[lane] + (LANES_END_X[lane] - LANES_START_X[lane]) * progress;

        // On booste un peu la descente (650.f au lieu de 500.f) pour la sensation de vitesse
        float posY = (HORIZON_Y + 100.f) + (650.f * progress);

        sprite.setPosition({posX, posY});
        return progress > 1.1f;
    }
};

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

    void jump() { if (state == PlayerState::RUN) { state = PlayerState::JUMP; vY = -18.f; jumpDelay = 0.3f; frameNum = 0; } }
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
        float targetX = LANES_END_X[lane];
        float currX = sprite.getPosition().x;
        sprite.setPosition({currX + (targetX - currX) * 0.15f, y});
        if (jumpDelay > 0.f) { jumpDelay -= dt; updateSpriteRect(jumpFrames[0]); return; }
        std::vector<sf::IntRect>* currentList = &runFrames;
        float speed = 0.12f;
        if (state == PlayerState::JUMP) { currentList = &jumpFrames; speed = 0.15f; vY += 0.45f; y += vY; if (y >= GROUND_Y) { y = GROUND_Y; vY = 0.f; state = PlayerState::RUN; } }
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

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Pointes of No Return");
    window.setFramerateLimit(60);

    sf::Texture pTex, oTex, texBtnReplay, texBtnQuit, skyTex, operaTex, roadTex, buildingTex;
    if (!pTex.loadFromFile("./assets/player_lvl1_pink.png")) return -1;
    if (!oTex.loadFromFile("./assets/obstacle.png")) return -1;
    if (!texBtnReplay.loadFromFile("./assets/REPLAY.png")) return -1;
    if (!texBtnQuit.loadFromFile("./assets/QUIT.png")) return -1;
    if (!skyTex.loadFromFile("./assets/bg_sky.png")) return -1;
    if (!operaTex.loadFromFile("./assets/opéra.png")) return -1;
    if (!roadTex.loadFromFile("./assets/roof_ground.png")) return -1;
    if (!buildingTex.loadFromFile("./assets/référence.png")) return -1;

    Player ballerine(pTex);
    Decor decor(skyTex, operaTex, buildingTex);
    Road route(roadTex);
    GameOverUI ui;
    ui.load("./assets/gameover.png");

    sf::FloatRect btnReplay({260.f, 380.f}, {200.f, 80.f});
    sf::FloatRect btnQuit({560.f, 380.f}, {200.f, 80.f});

    sf::Sprite spriteReplay(texBtnReplay);
    spriteReplay.setPosition(btnReplay.position);
    sf::Sprite spriteQuit(texBtnQuit);
    spriteQuit.setPosition(btnQuit.position);

    std::vector<std::unique_ptr<Obstacle>> obstacles;
    sf::Clock animClock, spawnTimer, deathTimer;
    GameState state = GameState::PLAYING;

    while (window.isOpen()) {
        float dt = animClock.restart().asSeconds();
        sf::Vector2f mouse = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (state == GameState::GAMEOVER_MENU && event->is<sf::Event::MouseButtonPressed>()) {
                if (btnReplay.contains(mouse)) {
                    state = GameState::PLAYING;
                    ballerine.reset();
                    decor.reset();
                    obstacles.clear();
                    ui.reset();
                    spawnTimer.restart();
                }
                else if (btnQuit.contains(mouse)) window.close();
            }

            if (state == GameState::PLAYING) {
                if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                    if (kp->code == sf::Keyboard::Key::Left && ballerine.lane > 0) ballerine.changeLane(ballerine.lane - 1);
                    if (kp->code == sf::Keyboard::Key::Right && ballerine.lane < 2) ballerine.changeLane(ballerine.lane + 1);
                    if (kp->code == sf::Keyboard::Key::Space) ballerine.jump();
                }
            }
        }

        ballerine.update(dt);

        if (state == GameState::PLAYING) {
            decor.update(dt, state);
            route.update(gameSpeed);

            // Spawn des ennemis (tous les 1.8s)
            if (spawnTimer.getElapsedTime().asSeconds() > 1.8f) {
                obstacles.push_back(std::make_unique<Obstacle>(oTex));
                spawnTimer.restart();
            }

            for (auto it = obstacles.begin(); it != obstacles.end();) {
                if ((*it)->update(gameSpeed)) {
                    it = obstacles.erase(it);
                } else {
                    // --- NOUVELLE COLLISION PLUS JUSTE ---
                    if ((*it)->lane == ballerine.lane && (*it)->progress > 0.92f && (*it)->progress < 1.05f) {
                        // si t'es a moins de 80px de haut, tu tapes l'obstacle
                        // plus ce chiffre est petit, plus c'est dur de sauter
                        // plus il est grand, plus c'est facile
                        if (ballerine.y > GROUND_Y - 80.f) {
                            ballerine.state = PlayerState::DIE;
                            deathTimer.restart();
                            state = GameState::BALLERINE_FALLING;
                            break;
                        }
                    }
                    ++it;
                }
            }
        }
        else if (state == GameState::BALLERINE_FALLING) {
            if (deathTimer.getElapsedTime().asSeconds() > 2.0f) {
                ui.alpha = 0;
                state = GameState::FADING_TO_GAMEOVER;
            }
        }
        else if (state == GameState::FADING_TO_GAMEOVER) {
            if (ui.blackScreen.getFillColor().a < 255) {
                ui.updateFade(dt, true);
            } else {
                ui.updateFade(dt, false);
                if (ui.bg && ui.bg->getColor().a == 255) state = GameState::GAMEOVER_MENU;
            }
        }

        window.clear(sf::Color(25, 25, 45));

        // Dessin
        decor.draw(window);
        route.draw(window);

        for (auto& o : obstacles) window.draw(o->sprite);
        window.draw(ballerine.sprite);

        if (state == GameState::FADING_TO_GAMEOVER || state == GameState::GAMEOVER_MENU) {
            window.draw(ui.blackScreen);
            if (ui.bg) window.draw(*ui.bg);
            if (state == GameState::GAMEOVER_MENU) {
                window.draw(spriteReplay);
                window.draw(spriteQuit);
            }
        }

        window.display();
    }
    return 0;
}