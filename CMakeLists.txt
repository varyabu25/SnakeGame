cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Требование п. 10: Компиляция с предупреждениями
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

# Вот эта строка ищет все .cpp файлы в папке src
file(GLOB SOURCES "src/*.cpp")

# Создаем исполняемый файл с именем snake_app
add_executable(snake_app ${SOURCES})