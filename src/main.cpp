#include <SFML/Graphics.hpp>

#include "Game.h"

#include <iostream>

int main()
{
    Luntik::Game* game = new Luntik::Game();
    int exit_code = game->run();

    std::cout << "Exit code: " << exit_code << std::endl;
    // std::cin.ignore();

    return exit_code;
}
