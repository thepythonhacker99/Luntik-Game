cmake_minimum_required(VERSION 3.21)
project(Luntik LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

set(ADDONS_DIR ./lib/sfml_addons)
set(RANDOM_DIR ./lib/random)
set(PERLIN_DIR ./lib/perlin)

file(GLOB_RECURSE sources
    "src/*.cpp"
    ${PERLIN_DIR}/SimplexNoise.cpp
)
add_executable(Luntik ${sources})

include_directories(${ADDONS_DIR})
include_directories(${RANDOM_DIR})
include_directories(${PERLIN_DIR})

target_link_libraries(Luntik PRIVATE sfml-graphics sfml-network)
set_property(TARGET Luntik PROPERTY CXX_STANDARD 17)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET Luntik POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Luntik> $<TARGET_FILE_DIR:Luntik> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS Luntik)
