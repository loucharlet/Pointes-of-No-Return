#include "SaveManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

static inline std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool SaveManager::load(Save& out, const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;

    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = trim(line.substr(0, pos));
        std::string val = trim(line.substr(pos + 1));
        try {
            if (key == "coll1") out.coll1 = std::stoi(val);
            else if (key == "coll2") out.coll2 = std::stoi(val);
            else if (key == "coll3") out.coll3 = std::stoi(val);
            else if (key == "coll4") out.coll4 = std::stoi(val);
            else if (key == "costomables") out.costomables = std::stoi(val);
            else if (key == "highscore") out.highscore = std::stoi(val);
            else if (key == "playerName") out.playerName = val;
            else if (key == "equippedCostume") out.equippedCostume = val;
            else if (key == "levelReached") out.levelReached = std::stoi(val);
        } catch (...) {
            // ignore parse errors and continue
        }
        }
        return true;
        }

        bool SaveManager::save(const Save& s, const std::string& path) {
        std::ofstream f(path, std::ios::trunc);
        if (!f.is_open()) return false;
        f << "coll1=" << s.coll1 << "\n";
        f << "coll2=" << s.coll2 << "\n";
        f << "coll3=" << s.coll3 << "\n";
        f << "coll4=" << s.coll4 << "\n";
        f << "costomables=" << s.costomables << "\n";
        f << "highscore=" << s.highscore << "\n";
        f << "playerName=" << s.playerName << "\n";
        f << "equippedCostume=" << s.equippedCostume << "\n";
        f << "levelReached=" << s.levelReached << "\n";
        return true;
        }

