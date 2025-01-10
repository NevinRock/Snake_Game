cmake_minimum_required(VERSION 3.10)
project(Snake_Game)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 查找 SDL2 和相关库
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# 添加宏以避免 SDL 重定义 main
add_definitions(-DSDL_MAIN_HANDLED)

# 添加可执行文件
add_executable(Snake_Game src/main.cpp)

# 链接 SDL2 和相关库
target_link_libraries(Snake_Game SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)
