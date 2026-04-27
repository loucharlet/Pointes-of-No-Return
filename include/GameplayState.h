#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H

#include "Scene.h"
#include "Constants.h"
#include "SaveManager.h"
#include "AppSettings.h"
#include "SettingsUI.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <optional>

class SceneManager;

class GameplayState : public Scene {
public:
    explicit GameplayState(SceneManager* scenes, int levelId = 0, const Save& save = Save(), const std::string& path = "", int slotIndex = -1);
    ~GameplayState();

    void onEnter() override;
    void onExit() override;
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    int levelId = 0; // 0=Paris, 1..4 = ballets
    SceneManager* scenes = nullptr;
    // textures & assets
    sf::Texture pTex, oTex, texBtnReplay, texBtnQuit, skyTex, operaTex, roadTex, bTex1, bTex2, bTex3, tArc, tGalerie, tNotreDame, tMoulin, texScoreIcon, texClouds, tLogo, tFullInventory, tFullInventoryBack;
    sf::Texture collTex1, collTex2, collTex3, collTex4, costomableTex;
    sf::Texture popupTex1, popupTex2, popupTex3, popupTex4, costomablePopupTex;

    // audio
    sf::Music levelMusic, gameOverMusic;
    sf::SoundBuffer bufCollec, bufDeath, bufStart;
    std::unique_ptr<sf::Sound> sfxCollec, sfxDeath, sfxStart;

    // game objects
    std::vector<const sf::Texture*> texVariations;
    class Decor* decor = nullptr;
    class Player* ballerine = nullptr;
    class Road* route = nullptr;
    class GameOverUI* ui = nullptr;

    std::vector<std::unique_ptr<class Collectible>> collectibles;
    std::vector<std::unique_ptr<class Obstacle>> obstacles;

    // UI
    sf::Font font;
    std::unique_ptr<sf::Text> scoreText;
    std::unique_ptr<sf::Sprite> spriteScoreIcon;
    std::unique_ptr<sf::Sprite> spriteLogo;
    std::unique_ptr<sf::Sprite> spriteFullIventory;
    std::unique_ptr<sf::Sprite> spriteFullIventoryBack;
    SettingsUI settingsUI;
    std::unique_ptr<sf::Sprite> sReplay, sQuit;

    // logic
    std::vector<sf::Texture*> collectibleTextures;
    std::vector<struct Popup> popups;

    sf::Clock spawnTimer, deathTimer, collectibleTimer;
    GameState state = GameState::PLAYING;
    int totalCollectiblesSpawned = 0;
    int score = 0;
    Save saveData;
    std::string slotPath;
    int slotIndex = -1;

    bool collectiblePending = false;
    int pendingLane = 0;
};

#endif