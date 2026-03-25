#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <SFML/Graphics.hpp>

// config window
const float WINDOW_WIDTH = 1000.f;
const float WINDOW_HEIGHT = 800.f;

//physics
const float HORIZON_Y = 220.f;
const float GROUND_Y = 750.f;
const float gameSpeed = 0.005f;

//lanes
const float LANES_END_X[] = {200.f, 500.f, 800.f};
const float LANES_START_X[] = {500.f, 500.f, 500.f};
const float PLAYER_LANES_END_X[] = {300.f, 500.f, 700.f};

//gamestates
enum class GameState {
    MAIN_MENU,
    LEVEL_1,
    LEVEL_2,
    LEVEL_3,
    PLAYING,
    BALLERINE_FALLING,
    FADING_TO_GAMEOVER,
    GAMEOVER_MENU,
    LEVEL_COMPLETE
};

enum class PlayerState { RUN, JUMP, MOVE, DIE };

#endif