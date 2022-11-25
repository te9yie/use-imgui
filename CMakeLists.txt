cmake_minimum_required(VERSION 3.14)
project(use-imgui)

include(FetchContent)

find_package(OpenGL REQUIRED)

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(imgui)

file(GLOB imgui_sources
    ${imgui_SOURCE_DIR}/*.h
    ${imgui_SOURCE_DIR}/*.cpp)
file(GLOB imgui_backends
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.*
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.*)
add_library(imgui STATIC ${imgui_sources} ${imgui_backends})
target_compile_features(imgui PUBLIC cxx_std_17)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC
    SDL2::SDL2-static
    OpenGL::GL OpenGL::GLU)

# SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.24.1
    GIT_SHALLOW TRUE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL)

# t9
FetchContent_Declare(
    t9
    GIT_REPOSITORY https://github.com/te9yie/cpp-t9
    GIT_TAG 2022-11-20
    GIT_SHALLOW TRUE)
set(T9_BUILD_SDL2 ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(t9)

file(GLOB_RECURSE sources src/*)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_SOURCE_DIR}/src FILES ${sources})

add_executable(${PROJECT_NAME} WIN32 ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W -Wall>)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:_CRTDBG_MAP_ALLOC>)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    t9::t9 t9::sdl2
    SDL2::SDL2-static SDL2::SDL2main
    OpenGL::GL OpenGL::GLU)