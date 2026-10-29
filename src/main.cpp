#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <thread>
#include <string>

#include "game.h"

int readKey() {
    static int escapeState = 0;
    fd_set input;
    FD_ZERO(&input);
    FD_SET(STDIN_FILENO, &input);
    timeval timeout = {0, 0};
    if (select(STDIN_FILENO + 1, &input, nullptr, nullptr, &timeout) <= 0) return -1;
    unsigned char key;
    if (read(STDIN_FILENO, &key, 1) != 1) return -2;
    if (key == 27) {
        escapeState = 1;
        return -1;
    }
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
    std::cout << "\033[2J\033[H"; 
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

    std::cout << "\033[?1049h\033[?25l\033[2J\033[H";

    bool paused = false;
    bool show_help = false;

    while (!state.is_game_over) {
        if (!show_help) {
            std::string frame = "\033[2J\033[H"; 
            frame += snake::get_field_string(state);
            if (paused) {
                frame += "ПАУЗА. Нажмите P для продолжения.\n";
            }
            std::cout << frame << std::flush;
        } else {
            std::cout << "\033[2J\033[H"; 
            snake::print_help();
            std::cout << "Нажмите H, чтобы вернуться в игру.\n" << std::flush;
        }

        int key = readKey();
        
        // Выход из игры (Q)
        if (key == 'q' || key == 'Q' || key == -2) {
            break; 
        }
        
        // Перезапуск игры (R)
        if (key == 'r' || key == 'R') {
            snake::init_game(state, gameMode, limit);
            paused = false;
            show_help = false;
            nextStep = std::chrono::steady_clock::now() + std::chrono::milliseconds(STEP_MS);
            continue; 
        }
        
        // Справка (H)
        if (key == 'h' || key == 'H') {
            show_help = !show_help;
            nextStep = std::chrono::steady_clock::now() + std::chrono::milliseconds(STEP_MS);
        } 
        // Пауза (P или пробел)
        else if (!show_help && (key == 'p' || key == 'P' || key == ' ')) {
            paused = !paused;
            nextStep = std::chrono::steady_clock::now() + std::chrono::milliseconds(STEP_MS);
        } 
        // Движение
        else if (!show_help && !paused && key != -1) {
            snake::process_input(state, static_cast<char>(key));
        }
        
        if (!paused && !show_help && std::chrono::steady_clock::now() >= nextStep) {
            snake::move_snake(state);
            nextStep = std::chrono::steady_clock::now() + std::chrono::milliseconds(STEP_MS);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    std::cout << "\033[?25h\033[?1049l";

    if (state.is_game_over) {
        snake::print_game_over(state);
    }

    return 0;
}