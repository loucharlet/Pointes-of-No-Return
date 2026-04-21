#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

namespace AssetLoader {

    // cache texture
    inline std::unordered_map<std::string, sf::Texture>& textureCache() {
        static std::unordered_map<std::string, sf::Texture> cache;
        return cache;
    }

    inline bool loadFont(sf::Font& font, const std::string& filename) {
        std::vector<std::string> candidates = {
            "./assets/" + filename,
            "./cmake-build-debug/assets/" + filename,
            filename
        };
        for (const auto& p : candidates) {
            if (font.openFromFile(p)) return true;
        }
        std::cerr << "Failed to load font: " << filename << std::endl;
        return false;
    }

    inline bool loadTexture(sf::Texture& tex, const std::string& filename) {
        auto& cache = textureCache();
        auto it = cache.find(filename);
        if (it != cache.end()) {
            tex = it->second;
            return true;
        }
        std::vector<std::string> candidates = {
            "./assets/img/" + filename,
            "./cmake-build-debug/assets/img/" + filename,
            filename
        };
        for (const auto& p : candidates) {
            if (cache[filename].loadFromFile(p)) {
                tex = cache[filename];
                return true;
            }
        }
        cache.erase(filename);
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }
}

#endif