#include "game.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

int readKey() {
    static int escapeState = 0;
    fd_set input;
    FD_ZERO(&input);
    FD_SET(STDIN_FILENO, &input);
    timeval timeout = {0, 0};
    if (select(STDIN_FILENO + 1, &input, nullptr, nullptr, &timeout) <= 0) return -1;
    unsigned char key;
    if (read(STDIN_FILENO, &key, 1) != 1) return -2;
    if (key == 27) { escapeState = 1; return -1; }
    if (escapeState == 1 && (key == '[' || key == 'O')) {
        escapeState = 2;
        return -1;
    }
    if (escapeState == 2) {
        escapeState = 0;
        if (key == 'A') return 'w';
        if (key == 'B') return 's';
        if (key == 'C') return 'd';
        if (key == 'D') return 'a';
        return -1;
    }
    escapeState = 0;
    if (key >= 'A' && key <= 'Z') key = key - 'A' + 'a';
    return key;
}

int readNumber(const char* prompt, int minimum, int maximum) {
    int number;
    while (true) {
        std::cout << prompt;
        if (std::cin >> number && number >= minimum && number <= maximum) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return number;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Введите целое число от " << minimum << " до " << maximum << ".\n";
    }
}

int main() {
    snake::print_help();
    
    int mode = readNumber("\nРежим: 1 — базовый, 2 — на время: ", 1, 2);
    snake::GameMode gameMode = (mode == 2) ? snake::GameMode::Timed : snake::GameMode::Basic;
    
    int limit = 0;
    if (gameMode == snake::GameMode::Timed) {
        limit = readNumber("Число ходов n (1..1000): ", 1, 1000);
    }
    
    termios oldSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    termios settings = oldSettings;
    settings.c_lflag &= ~(ICANON | ECHO);
    settings.c_cc[VMIN] = 0;
    settings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    
    snake::GameState state;
    snake::init_game(state, gameMode, limit);
    
    const int STEP_MS = 250;
    auto nextStep = std::chrono::steady_clock::now() + std::chrono::milliseconds(STEP_MS);
    
    std::cout << "\033[?1049h\033[?25l\033[2J"; 
    
    while (!state.is_game_over) {
        snake::print_field(state);
        
        int key = readKey();
        if (key == 'q' || key == -2) break;
        if (key != -1) {
            snake::process_input(state, static_cast<char>(key));
        }
        
        if (std::chrono::steady_clock::now() >= nextStep) {
            snake::move_snake(state);
            nextStep = std::chrono::steady_clock::now() + std::chrono::milliseconds(STEP_MS);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    std::cout << "\033[?25h\033[?1049l"; 
    
    if (state.is_game_over) {
        snake::print_game_over(state);
    }
    
    return 0;
}