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

    // Chemins de recherche
    inline std::vector<std::string> getCandidates(const std::string& filename, const std::string& type) {
        return {
            "./assets/" + type + "/" + filename,
            "./cmake-build-debug/assets/" + type + "/" + filename,
            "./assets/" + filename,
            "./cmake-build-debug/assets/" + filename,
            filename
        };
    }

    // Chargement unique (Texture)
    inline sf::Texture* getTexture(const std::string& filename) {
        auto& cache = textureCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];

        for (const auto& path : getCandidates(filename, "img")) {
            if (cache[filename].loadFromFile(path)) return &cache[filename];
        }
        cache.erase(filename);
        std::cerr << "!!! Failed to load texture: " << filename << std::endl;
        return nullptr;
    }

    // Chargement unique (Font)
    inline sf::Font* getFont(const std::string& filename) {
        auto& cache = fontCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];

        for (const auto& path : getCandidates(filename, "")) {
            if (cache[filename].openFromFile(path)) return &cache[filename];
        }
        cache.erase(filename);
        std::cerr << "!!! Failed to load font: " << filename << std::endl;
        return nullptr;
    }

    // Chargement unique (Sound)
    inline sf::SoundBuffer* getSoundBuffer(const std::string& filename) {
        auto& cache = soundCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];

        for (const auto& path : getCandidates(filename, "audio")) {
            if (cache[filename].loadFromFile(path)) return &cache[filename];
        }
        cache.erase(filename);
        std::cerr << "!!! Failed to load sound: " << filename << std::endl;
        return nullptr;
    }

    // Préchargement de tout le dossier assets
    inline void preloadAll() {
        std::cout << "--- Preloading assets... ---" << std::endl;
        
        std::vector<std::string> basePaths = {"./assets", "./cmake-build-debug/assets", "../assets"};
        std::string assetsRoot = "";
        for (const auto& bp : basePaths) {
            if (std::filesystem::exists(bp)) {
                assetsRoot = bp;
                break;
            }
        }

        if (assetsRoot.empty()) {
            std::cerr << "!!! Could not find assets folder!" << std::endl;
            return;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsRoot)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::string name = entry.path().filename().string();
                
                // On charge selon l'extension
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                    getTexture(name);
                } else if (ext == ".ogg" || ext == ".wav") {
                    getSoundBuffer(name);
                } else if (ext == ".ttf") {
                    getFont(name);
                }
            }
        }
        std::cout << "--- Assets preloaded! (" << textureCache().size() << " textures, " 
                  << soundCache().size() << " sounds) ---" << std::endl;
    }

    // Compatibilité avec l'ancien code (pour ne pas tout casser d'un coup)
    inline bool loadTexture(sf::Texture& tex, const std::string& filename) {
        sf::Texture* cached = getTexture(filename);
        if (cached) {
            tex = *cached; // Copie pour compatibilité
            return true;
        }
        return false;
    }

    inline bool loadFont(sf::Font& font, const std::string& filename) {
        sf::Font* cached = getFont(filename);
        if (cached) {
            // Les sf::Font ne sont pas facilement copiables, on utilise l'ancien système ou on renvoie true si déjà là
            return font.openFromFile(getCandidates(filename, "")[0]); 
        }
        return false;
    }
}

#endif
