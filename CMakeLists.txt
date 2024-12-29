cmake_minimum_required(VERSION 3.8)

project(Tower_Defense_Game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    $<$<CONFIG:Release>:-Os>
    $<$<CONFIG:Release>:-Wall>
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 设置库文件的输出目录（静态库和动态库）
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 设置不同构建类型的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)

add_definitions(-DPROJECT_DIR="${CMAKE_SOURCE_DIR}")

# 添加SDL库
add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(TowerDefenseGame ${SOURCES})

target_include_directories(TowerDefenseGame PRIVATE external/nlohmann_json)
target_include_directories(TowerDefenseGame PRIVATE external/SDL/include)
target_include_directories(TowerDefenseGame PRIVATE external/SDL_image/include)
target_include_directories(TowerDefenseGame PRIVATE external/SDL_ttf/include)
target_include_directories(TowerDefenseGame PRIVATE external/SDL_mixer/include)

target_link_directories(TowerDefenseGame PRIVATE external/SDL_ttf/lib/x64)
target_link_directories(TowerDefenseGame PRIVATE external/SDL_mixer/lib/x64)
target_link_libraries(TowerDefenseGame PRIVATE SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)

# 复制动态库到运行目录
file(COPY external/SDL_mixer/lib/x64/SDL2_mixer.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
file(COPY external/SDL_ttf/lib/x64/SDL2_ttf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)