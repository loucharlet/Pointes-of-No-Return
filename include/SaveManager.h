#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>

struct Save {
    int coll1 = 0; // Swan Lake
    int coll2 = 0; // Nutcracker
    int coll3 = 0; // La Sylphide
    int coll4 = 0; // Coppelia
    int costomables = 0; // Costume unlock items
    int highscore = 0;
    std::string playerName;
    std::string equippedCostume = "ballet"; // current equipped costume
    int levelReached = 0;
};

class SaveManager {
public:
    // Load save from file (default "save.txt").return true si fichier trouvé et parsé
    static bool load(Save& out, const std::string& path = "save.txt");

    // Save data to file (default "save.txt"). retourne true si succès
    static bool save(const Save& s, const std::string& path = "save.txt");
};

#endif
