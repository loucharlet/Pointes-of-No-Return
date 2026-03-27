cmake_minimum_required(VERSION 3.20)
project(Pointes_of_No_Return)

set(CMAKE_CXX_STANDARD 20)

include_directories(include "/opt/homebrew/include")
link_directories("/opt/homebrew/lib")

add_executable(Pointes_of_No_Return
        src/main.cpp
        include/Constants.h
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
)

target_link_libraries(Pointes_of_No_Return sfml-graphics sfml-window sfml-system sfml-audio)