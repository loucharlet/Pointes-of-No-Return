cmake_minimum_required(VERSION 3.20)
project(Pointes_of_No_Return)

set(CMAKE_CXX_STANDARD 20)

include_directories(include "/opt/homebrew/include")
link_directories("/opt/homebrew/lib")

add_executable(Pointes_of_No_Return
        src/main.cpp
        include/Constants.h
        include/AppSettings.h
        include/SettingsUI.h
        src/Player.cpp
        include/Player.h
        src/Decor.cpp
        include/Decor.h
        src/SideBuilding.cpp
        include/SideBuilding.h
        src/Road.cpp
        include/Road.h
        src/Obstacle.cpp
        include/Obstacle.h
        include/GameOverUI.h
        include/Collectible.h
        src/SaveManager.cpp
        src/SceneManager.cpp
        src/GameplayState.cpp
        include/Scene.h
        include/SceneManager.h
        include/GameplayState.h
        include/SaveManager.h
        include/LevelSelectState.h
        include/Menu1State.h
        include/Menu2State.h
        include/CostumeState.h
        src/LevelSelectState.cpp
        src/Menu1State.cpp
        src/Menu2State.cpp
        src/DressingState.cpp
        src/RulesState.cpp
        src/CostumeState.cpp
)

target_link_libraries(Pointes_of_No_Return sfml-graphics sfml-window sfml-system sfml-audio)