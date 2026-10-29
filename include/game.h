#pragma once

#include <vector>
#include <utility>
#include <string>

namespace snake {

inline constexpr int kFieldSize = 10;

using Point = std::pair<int, int>;

enum class Direction { Up, Down, Left, Right };
enum class GameMode { Basic, Timed };

struct GameState {
    std::vector<Point> snake;
    Point food;
    int score;
    bool is_game_over;
    bool is_victory;
    Direction current_dir;
    GameMode mode;
    int moves;
    int move_limit;
    Point bonus;
    int bonus_life;
};

void print_help();
std::string get_field_string(const GameState& state);
void print_game_over(const GameState& state);
void init_game(GameState& state, GameMode mode, int limit);
void spawn_food(GameState& state);
void process_input(GameState& state, char input);
void move_snake(GameState& state);

} // namespace snake