#include "../include/game.h"
#include <cassert>
#include <iostream>

void test_movement() {
    snake::GameState state;
    snake::init_game(state, snake::GameMode::Basic, 0);
    
    // Змейка должна быть в центре (5,5)
    assert(state.snake[0].first == 5 && state.snake[0].second == 5);
    assert(state.snake.size() == 1);
    
    // Двигаем вправо
    state.current_dir = snake::Direction::Right;
    snake::move_snake(state);
    
    // Голова должна сместиться на (6,5)
    assert(state.snake[0].first == 6 && state.snake[0].second == 5);
    assert(state.snake.size() == 1); // Длина не изменилась (не ели)
}

void test_eating() {
    snake::GameState state;
    snake::init_game(state, snake::GameMode::Basic, 0);
    
    // Ставим еду прямо перед змейкой
    state.food = {6, 5};
    
    // Двигаем вправо
    state.current_dir = snake::Direction::Right;
    snake::move_snake(state);
    
    // Должны съесть еду
    assert(state.score == 1);
    assert(state.snake.size() == 2); // Змейка выросла
}

void test_collision() {
    snake::GameState state;
    snake::init_game(state, snake::GameMode::Basic, 0);
    
    // Ставим змейку у правой стены
    state.snake[0] = {9, 5};
    state.current_dir = snake::Direction::Right;
    snake::move_snake(state);
    
    // Должны врезаться в стену
    assert(state.is_game_over == true);
}

int main() {
    test_movement();
    test_eating();
    test_collision();
    std::cout << "Все тесты пройдены успешно!" << std::endl;
    return 0;
}