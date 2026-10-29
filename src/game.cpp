#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

namespace snake {

bool is_point_in_snake(const GameState& state, const Point& p) {
    for (const auto& segment : state.snake) {
        if (segment == p) return true;
    }
    return false;
}

bool get_random_free_cell(const GameState& state, Point& out_point, Point avoid) {
    std::vector<Point> free_cells;
    for (int y = 0; y < kFieldSize; ++y) {
        for (int x = 0; x < kFieldSize; ++x) {
            Point p = {x, y};
            if (!is_point_in_snake(state, p) && p != state.food && p != avoid) {
                free_cells.push_back(p);
            }
        }
    }
    if (free_cells.empty()) return false;
    out_point = free_cells[std::rand() % free_cells.size()];
    return true;
}

void init_game(GameState& state, GameMode mode, int limit) {
    state.snake.clear();
    int center = kFieldSize / 2;
    state.snake.push_back({center, center});
    state.score = 0;
    state.is_game_over = false;
    state.is_victory = false;
    state.current_dir = Direction::Right;
    state.mode = mode;
    state.moves = 0;
    state.move_limit = limit;
    state.bonus_life = 0;
    
    std::srand(std::time(nullptr));
    spawn_food(state);
}

void spawn_food(GameState& state) {
    Point new_food;
    if (get_random_free_cell(state, new_food, state.bonus)) {
        state.food = new_food;
    }
}

void print_help() {
    std::cout << "\n=== Правила игры «Змейка» ===\n"
              << "Цель: набрать как можно больше очков, собирая еду.\n"
              << "Поле 10x10. Начальная длина змейки — 1, старт — в центре.\n"
              << "@ — голова, o — тело, * — еда, $ — бонус.\n"
              << "Еда даёт +1 очко и увеличивает длину на 1.\n"
              << "Столкновение со стеной или телом означает проигрыш.\n"
              << "Заполнение всего поля означает победу.\n"
              << "Управление: стрелки или W — вверх, S — вниз, A — влево, D — вправо.\n"
              << "Английская раскладка, без Enter. Змейка движется автоматически.\n"
              << "Ход — смена направления. Движение прямо не расходует ходы.\n"
              << "H — справка и остановка; H — обратно. P/пробел — пауза, R — заново, Q — выход.\n"
              << "В базовом режиме число ходов не ограничено, бонусов нет.\n"
              << "В режиме «На время» игра длится не более n ходов.\n"
              << "После каждого 10-го хода появляется бонус (+3 очка, без роста).\n"
              << "Он доступен до конца перемещения с третьей следующей сменой направления.\n"
              << "При движении прямо бонус не стареет; повтор той же клавиши — не ход.\n"
              << "При достижении n ходов игра завершается с текущим счётом.\n\n";
}

std::string get_field_string(const GameState& state) {
    std::string buffer;
    
    buffer += "Счет: " + std::to_string(state.score);
    if (state.mode == GameMode::Timed) {
        buffer += " | Ходы: " + std::to_string(state.moves) + "/" + std::to_string(state.move_limit);
    }
    if (state.bonus_life > 0) {
        buffer += " | Бонус сгорит через: " + std::to_string(state.bonus_life);
    }
    buffer += "          \n"; 

    // Верхняя стенка (22 символа: 10*2 + 2 края)
    buffer += "######################\n";

    for (int y = 0; y < kFieldSize; ++y) {
        buffer += "#"; // Левая стенка
        for (int x = 0; x < kFieldSize; ++x) {
            Point current = {x, y};
            // Каждая клетка состоит из символа и пробела (ширина 2)
            if (state.snake.front() == current) buffer += "@ ";
            else if (is_point_in_snake(state, current)) buffer += "o ";
            else if (state.food == current) buffer += "* ";
            else if (state.bonus_life > 0 && state.bonus == current) buffer += "$ ";
            else buffer += "  ";
        }
        buffer += "#\n"; // Правая стенка
    }

    // Нижняя стенка
    buffer += "######################\n";
    
    return buffer;
}

void print_game_over(const GameState& state) {
    std::cout << "\n==============================\n";
    if (state.is_victory) {
        std::cout << "           ПОБЕДА!            \n";
    } else {
        std::cout << "         ИГРА ОКОНЧЕНА        \n";
    }
    std::cout << "==============================\n";
    std::cout << "Ваш итоговый счет: " << state.score << "\n";
}

void process_input(GameState& state, char input) {
    Direction old_dir = state.current_dir;
    
    switch (input) {
        case 'w': case 'W': state.current_dir = Direction::Up; break;
        case 's': case 'S': state.current_dir = Direction::Down; break;
        case 'a': case 'A': state.current_dir = Direction::Left; break;
        case 'd': case 'D': state.current_dir = Direction::Right; break;
    }
    
    if (old_dir != state.current_dir) {
        state.moves++;
    }
}

void move_snake(GameState& state) {
    Point head = state.snake.front();
    Point new_head = head;

    switch (state.current_dir) {
        case Direction::Up:    new_head.second -= 1; break;
        case Direction::Down:  new_head.second += 1; break;
        case Direction::Left:  new_head.first -= 1;  break;
        case Direction::Right: new_head.first += 1;  break;
    }

    if (new_head.first < 0 || new_head.first >= kFieldSize ||
        new_head.second < 0 || new_head.second >= kFieldSize) {
        state.is_game_over = true;
        return;
    }

    for (size_t i = 0; i < state.snake.size() - 1; ++i) {
        if (state.snake[i] == new_head) {
            state.is_game_over = true;
            return;
        }
    }

    state.snake.insert(state.snake.begin(), new_head);

    if (state.bonus_life > 0) {
        if (new_head == state.bonus) {
            state.score += 3;
            state.bonus_life = 0;
        } else {
            state.bonus_life--;
        }
    }

    if (new_head == state.food) {
        state.score += 1;
        if (state.snake.size() == kFieldSize * kFieldSize) {
            state.is_victory = true;
            state.is_game_over = true;
            return;
        }
        spawn_food(state);
    } else {
        state.snake.pop_back();
    }
    
    if (state.mode == GameMode::Timed && state.moves > 0 && state.moves % 10 == 0) {
        if (state.bonus_life == 0) {
            get_random_free_cell(state, state.bonus, {-1, -1});
            state.bonus_life = 3;
        }
    }

    if (state.mode == GameMode::Timed && state.moves >= state.move_limit) {
        state.is_game_over = true;
    }
}

} // namespace snake