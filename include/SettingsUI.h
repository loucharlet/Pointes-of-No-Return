#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include "Constants.h"
#include "AssetLoader.h"
#include "AppSettings.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <memory>

class SettingsUI {
public:
    void init(sf::Font& font) {
        font_ = &font;

        AssetLoader::loadTexture(texSettingsIcon_, "settings_icon.png");
        AssetLoader::loadTexture(texSettingsBg_, "settingsbg.png");

        spriteSettings_ = std::make_unique<sf::Sprite>(texSettingsIcon_);
        spriteSettings_->setScale({0.35f, 0.35f});
        spriteSettings_->setPosition({WINDOW_WIDTH - 80.f, 30.f});

        bigPanel_.setSize({600.f, 500.f});
        bigPanel_.setTexture(&texSettingsBg_);
        bigPanel_.setOutlineThickness(5.f);
        bigPanel_.setOutlineColor(sf::Color::Black);
        bigPanel_.setOrigin({300.f, 250.f});
        bigPanel_.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 60.f});

        pTitle_ = std::make_unique<sf::Text>(*font_, "SETTINGS", 45);
        pMusic_ = std::make_unique<sf::Text>(*font_, AppSettings::musicOn ? "Music: ON" : "Music: OFF", 30);
        pSfx_ = std::make_unique<sf::Text>(*font_, AppSettings::sfxOn ? "SFX: ON" : "SFX: OFF", 30);
        pVol_ = std::make_unique<sf::Text>(*font_, "Volume: " + std::to_string((int)AppSettings::volume), 30);
        pClose_ = std::make_unique<sf::Text>(*font_, "X", 25);
        pQuit_ = std::make_unique<sf::Text>(*font_, "Quit", 26);
        pRestart_ = std::make_unique<sf::Text>(*font_, "Restart", 26);

        pTitle_->setPosition({WINDOW_WIDTH / 2.f - 100.f, 175.f});
        pMusic_->setPosition({WINDOW_WIDTH / 2.f - 100.f, 280.f});
        pSfx_->setPosition({WINDOW_WIDTH / 2.f - 100.f, 340.f});
        pVol_->setPosition({WINDOW_WIDTH / 2.f - 100.f, 400.f});
        pClose_->setPosition({WINDOW_WIDTH / 2.f + 270.f, 100.f});
        pClose_->setFillColor(sf::Color::Red);
        pRestart_->setPosition({WINDOW_WIDTH / 2.f - 100.f, 460.f});
        pQuit_->setPosition({WINDOW_WIDTH / 2.f + 120.f, 460.f});

        pTitle_->setOutlineColor(sf::Color::Black);
        pTitle_->setOutlineThickness(3.f);
        pMusic_->setOutlineColor(sf::Color::Black);
        pMusic_->setOutlineThickness(3.f);
        pSfx_->setOutlineColor(sf::Color::Black);
        pSfx_->setOutlineThickness(3.f);
        pVol_->setOutlineColor(sf::Color::Black);
        pVol_->setOutlineThickness(3.f);
        pQuit_->setOutlineColor(sf::Color::Black);
        pQuit_->setOutlineThickness(3.f);
        pRestart_->setOutlineColor(sf::Color::Black);
        pRestart_->setOutlineThickness(3.f);
    }

    void setGameActionsEnabled(bool enabled) { showGameActions_ = enabled; }

    void setAudioHooks(
        const std::function<void(bool)>& onMusicChanged,
        const std::function<void(float)>& onVolumeChanged
    ) {
        onMusicChanged_ = onMusicChanged;
        onVolumeChanged_ = onVolumeChanged;
    }

    bool isOpen() const { return showSettingsPanel_; }

    bool handleClick(
        const sf::Vector2f& mPos,
        const std::function<void()>& onQuit,
        const std::function<void()>& onRestart
    ) {
        if (showSettingsPanel_) {
            if (pClose_ && pClose_->getGlobalBounds().contains(mPos)) {
                showSettingsPanel_ = false;
                return true;
            }
            if (pMusic_ && pMusic_->getGlobalBounds().contains(mPos)) {
                AppSettings::musicOn = !AppSettings::musicOn;
                pMusic_->setString(AppSettings::musicOn ? "Music: ON" : "Music: OFF");
                if (onMusicChanged_) onMusicChanged_(AppSettings::musicOn);
                return true;
            }
            if (pVol_ && pVol_->getGlobalBounds().contains(mPos)) {
                AppSettings::volume = (AppSettings::volume >= 100.f) ? 0.f : (AppSettings::volume + 20.f);
                pVol_->setString("Volume: " + std::to_string((int)AppSettings::volume));
                if (onVolumeChanged_) onVolumeChanged_(AppSettings::volume);
                return true;
            }
            if (pSfx_ && pSfx_->getGlobalBounds().contains(mPos)) {
                AppSettings::sfxOn = !AppSettings::sfxOn;
                pSfx_->setString(AppSettings::sfxOn ? "SFX: ON" : "SFX: OFF");
                return true;
            }
            if (showGameActions_) {
                if (pRestart_ && pRestart_->getGlobalBounds().contains(mPos)) {
                    if (onRestart) onRestart();
                    return true;
                }
                if (pQuit_ && pQuit_->getGlobalBounds().contains(mPos)) {
                    if (onQuit) onQuit();
                    return true;
                }
            }
            // panel open: swallow clicks so menus don't trigger underneath
            return true;
        }

        if (spriteSettings_ && spriteSettings_->getGlobalBounds().contains(mPos)) {
            showSettingsPanel_ = true;
            return true;
        }

        return false;
    }

    void draw(sf::RenderWindow& window) {
        if (spriteSettings_) window.draw(*spriteSettings_);
        if (showSettingsPanel_) {
            window.draw(bigPanel_);
            if (pTitle_) window.draw(*pTitle_);
            if (pMusic_) window.draw(*pMusic_);
            if (pSfx_) window.draw(*pSfx_);
            if (pVol_) window.draw(*pVol_);
            if (pClose_) window.draw(*pClose_);
            if (showGameActions_) {
                if (pRestart_) window.draw(*pRestart_);
                if (pQuit_) window.draw(*pQuit_);
            }
        }
    }

private:
    sf::Font* font_ = nullptr;

    sf::Texture texSettingsIcon_;
    sf::Texture texSettingsBg_;

    std::unique_ptr<sf::Sprite> spriteSettings_;

    sf::RectangleShape bigPanel_;
    std::unique_ptr<sf::Text> pTitle_, pMusic_, pSfx_, pVol_, pClose_, pQuit_, pRestart_;

    bool showSettingsPanel_ = false;
    bool showGameActions_ = false;

    std::function<void(bool)> onMusicChanged_;
    std::function<void(float)> onVolumeChanged_;
};

#endif
