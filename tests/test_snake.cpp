#include "../include/game.h"
#include <cassert>
#include <iostream>

using namespace snake;

void test_initialization() {
    GameState state;
    init_game(state, GameMode::Basic, 0);
    
    // Проверка начального состояния
    assert(state.snake.size() == 1);
    assert(state.snake[0].first == 5 && state.snake[0].second == 5); // Центр
    assert(state.score == 0);
    assert(!state.is_game_over);
    assert(state.current_dir == Direction::Right);
}

void test_movement() {
    GameState state;
    init_game(state, GameMode::Basic, 0);
    
    // Двигаем вправо
    state.current_dir = Direction::Right;
    move_snake(state);
    
    // Голова должна сместиться на (6,5)
    assert(state.snake[0].first == 6 && state.snake[0].second == 5);
    assert(state.snake.size() == 1);
}

void test_eating() {
    GameState state;
    init_game(state, GameMode::Basic, 0);
    
    // Ставим еду прямо перед змейкой
    state.food = {6, 5};
    
    // Двигаем вправо
    state.current_dir = Direction::Right;
    move_snake(state);
    
    // Должны съесть еду
    assert(state.score == 1);
    assert(state.snake.size() == 2); // Змейка выросла
}

void test_collision_wall() {
    GameState state;
    init_game(state, GameMode::Basic, 0);
    
    // Ставим змейку у правой стены
    state.snake[0] = {9, 5};
    state.current_dir = Direction::Right;
    move_snake(state);
    
    // Должны врезаться в стену
    assert(state.is_game_over == true);
}

void test_timed_mode_moves() {
    GameState state;
    init_game(state, GameMode::Timed, 10); // Лимит 10 ходов
    
    // Проверяем, что счетчик ходов увеличивается при смене направления
    process_input(state, 'w');
    assert(state.moves == 1);
    
    process_input(state, 'w'); // Повтор не считается
    assert(state.moves == 1);
    
    process_input(state, 'a');
    assert(state.moves == 2);
}

int main() {
    test_initialization();
    test_movement();
    test_eating();
    test_collision_wall();
    test_timed_mode_moves();
    
    std::cout << "Все тесты пройдены успешно!" << std::endl;
    return 0;
}