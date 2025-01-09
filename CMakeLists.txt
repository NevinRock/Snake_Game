cmake_minimum_required(VERSION 3.10)
project(Snake_Game)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 查找 SDL2 和 SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)


# 添加可执行文件 (将 main.cpp 改为你实际的文件名)
add_executable(Snake_Game src/main.cpp)

# 链接 SDL2 和 SDL2_image 库
target_link_libraries(Snake_Game SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)
