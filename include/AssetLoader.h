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

    // Helper to find the base assets directory
    inline std::string findAssetsRoot() {
        std::vector<std::string> basePaths = {".", "..", "../..", "./cmake-build-debug", "../../cmake-build-debug"};
        for (const auto& bp : basePaths) {
            if (std::filesystem::exists(bp + "/assets")) return bp + "/assets";
        }
        return "";
    }

    // Helper to find a specific file path (for individual loads)
    inline std::string findPath(const std::string& filename, const std::vector<std::string>& subDirs) {
        std::string root = findAssetsRoot();
        if (root.empty()) return "";
        
        // Remove "assets" from root to use subDirs like "/assets/img/"
        std::string base = root.substr(0, root.find_last_of("/\\") - 6); 

        std::vector<std::string> searchPaths = {root, root + "/img", root + "/audio", root + "/fonts"};
        for (const auto& sp : searchPaths) {
            std::string p = sp + "/" + filename;
            if (std::filesystem::exists(p)) return p;
        }
        
        // Recursive search as fallback
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
            if (entry.is_regular_file() && entry.path().filename() == filename) {
                return entry.path().string();
            }
        }
        return "";
    }

    // Caches
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

    inline sf::Texture* getTexture(const std::string& filename) {
        auto& cache = textureCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];
        std::string path = findPath(filename, {});
        if (!path.empty() && cache[filename].loadFromFile(path)) return &cache[filename];
        cache.erase(filename);
        return nullptr;
    }

    inline sf::Font* getFont(const std::string& filename) {
        auto& cache = fontCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];
        std::string path = findPath(filename, {});
        if (!path.empty() && cache[filename].openFromFile(path)) return &cache[filename];
        cache.erase(filename);
        return nullptr;
    }

    inline sf::SoundBuffer* getSoundBuffer(const std::string& filename) {
        auto& cache = soundCache();
        if (cache.find(filename) != cache.end()) return &cache[filename];
        std::string path = findPath(filename, {});
        if (!path.empty() && cache[filename].loadFromFile(path)) return &cache[filename];
        cache.erase(filename);
        return nullptr;
    }

    inline bool loadTexture(sf::Texture& tex, const std::string& filename) {
        sf::Texture* cached = getTexture(filename);
        if (cached) { tex = *cached; return true; }
        return false;
    }

    inline bool loadFont(sf::Font& font, const std::string& filename) {
        sf::Font* cached = getFont(filename);
        if (cached) return font.openFromFile(findPath(filename, {}));
        return false;
    }

    inline std::string findAudioPath(const std::string& filename) {
        return findPath(filename, {});
    }

    // Restore full preloading
    inline void preloadAll() {
        std::string root = findAssetsRoot();
        if (root.empty()) {
            std::cerr << "!!! Assets directory not found!" << std::endl;
            return;
        }

        std::cout << "--- Preloading assets from: " << root << " ---" << std::endl;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
            if (!entry.is_regular_file()) continue;
            std::string name = entry.path().filename().string();
            std::string ext = entry.path().extension().string();

            if (ext == ".png" || ext == ".jpg") {
                (void)textureCache()[name].loadFromFile(entry.path().string());
            } else if (ext == ".ogg" || ext == ".wav") {
                (void)soundCache()[name].loadFromFile(entry.path().string());
            } else if (ext == ".ttf") {
                (void)fontCache()[name].openFromFile(entry.path().string());
            }
        }
        std::cout << "--- Preload complete (" << textureCache().size() << " textures) ---" << std::endl;
    }

    inline void scaleToCoverLeft(sf::Sprite& sprite, float windowW, float windowH) {
        auto texSize = sprite.getTexture().getSize();
        if (texSize.x == 0 || texSize.y == 0) return;
        float scaleX = windowW / (float)texSize.x;
        float scaleY = windowH / (float)texSize.y;
        float scale = std::max(scaleX, scaleY);
        sprite.setScale({scale, scale});
        sprite.setPosition({0.f, 0.f});
    }

    inline void scaleToCoverCenter(sf::Sprite& sprite, float windowW, float windowH) {
        auto texSize = sprite.getTexture().getSize();
        if (texSize.x == 0 || texSize.y == 0) return;
        float scaleX = windowW / (float)texSize.x;
        float scaleY = windowH / (float)texSize.y;
        float scale = std::max(scaleX, scaleY);
        sprite.setScale({scale, scale});
        
        float newWidth = (float)texSize.x * scale;
        float newHeight = (float)texSize.y * scale;
        sprite.setPosition({(windowW - newWidth) / 2.f, (windowH - newHeight) / 2.f});
    }
}

#endif
