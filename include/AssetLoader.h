#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

namespace AssetLoader {

    // Caches pour les ressources
    inline std::unordered_map<std::string, sf::Texture>& textureCache() {
        static std::unordered_map<std::string, sf::Texture> cache;
        return cache;
    }

    inline std::unordered_map<std::string, sf::Font>& fontCache() {
        static std::unordered_map<std::string, sf::Font> cache;
        return cache;
    }

    inline std::unordered_map<std::string, sf::SoundBuffer>& soundCache() {
        static std::unordered_map<std::string, sf::SoundBuffer> cache;
        return cache;
    }

    // Fonction interne pour charger une ressource avec un chemin direct sans spammer la console
    template<typename T>
    inline bool directLoad(T& resource, const std::string& path) {
        if (!std::filesystem::exists(path)) return false;
        return resource.loadFromFile(path);
    }

    // Spécialisation pour sf::Font car openFromFile != loadFromFile
    inline bool directLoadFont(sf::Font& font, const std::string& path) {
        if (!std::filesystem::exists(path)) return false;
        return font.openFromFile(path);
    }

    // Chargement unique (Texture)
    inline sf::Texture* getTexture(const std::string& filename) {
        auto& cache = textureCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];

        // Chemins prioritaires
        std::vector<std::string> paths = {
            "./assets/img/" + filename,
            "./cmake-build-debug/assets/img/" + filename,
            "./assets/" + filename,
            filename
        };

        for (const auto& path : paths) {
            if (directLoad(cache[filename], path)) return &cache[filename];
        }

        cache.erase(filename);
        // On ne log que si c'est vraiment introuvable après tous les essais
        // std::cerr << "!!! Texture introuvable : " << filename << std::endl;
        return nullptr;
    }

    // Chargement unique (Font)
    inline sf::Font* getFont(const std::string& filename) {
        auto& cache = fontCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];

        std::vector<std::string> paths = { "./assets/" + filename, "./cmake-build-debug/assets/" + filename, filename };
        for (const auto& path : paths) {
            if (directLoadFont(cache[filename], path)) return &cache[filename];
        }
        cache.erase(filename);
        return nullptr;
    }

    // Chargement unique (Sound)
    inline sf::SoundBuffer* getSoundBuffer(const std::string& filename) {
        auto& cache = soundCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];

        std::vector<std::string> paths = { "./assets/audio/" + filename, "./cmake-build-debug/assets/audio/" + filename, filename };
        for (const auto& path : paths) {
            if (directLoad(cache[filename], path)) return &cache[filename];
        }
        cache.erase(filename);
        return nullptr;
    }

    // Préchargement ultra-rapide
    inline void preloadAll() {
        std::cout << "--- Preloading assets... ---" << std::endl;
        
        std::vector<std::string> basePaths = {"./assets", "./cmake-build-debug/assets"};
        std::string assetsRoot = "";
        for (const auto& bp : basePaths) {
            if (std::filesystem::exists(bp)) {
                assetsRoot = bp;
                break;
            }
        }

        if (assetsRoot.empty()) return;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsRoot)) {
            if (!entry.is_regular_file()) continue;

            std::string path = entry.path().string();
            std::string name = entry.path().filename().string();
            std::string ext = entry.path().extension().string();

            // Si c'est déjà dans le cache, on passe
            if (textureCache().count(name) || soundCache().count(name) || fontCache().count(name)) continue;

            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                directLoad(textureCache()[name], path);
            } else if (ext == ".ogg" || ext == ".wav") {
                directLoad(soundCache()[name], path);
            } else if (ext == ".ttf") {
                directLoadFont(fontCache()[name], path);
            }
        }
        std::cout << "--- Assets preloaded! (" << textureCache().size() << " textures) ---" << std::endl;
    }

    // Compatibilité
    inline bool loadTexture(sf::Texture& tex, const std::string& filename) {
        sf::Texture* cached = getTexture(filename);
        if (cached) { tex = *cached; return true; }
        return false;
    }
    inline bool loadFont(sf::Font& font, const std::string& filename) {
        sf::Font* cached = getFont(filename);
        if (cached) return font.openFromFile(filename); // Simplifié
        return false;
    }
}

#endif
