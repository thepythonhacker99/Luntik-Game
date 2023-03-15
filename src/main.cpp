#include <SFML/Graphics.hpp>

#include "Game.h"

#include <iostream>

int main(int argc, char** argv)
{
    std::string ip = "0";

    if (argc == 2) {
        ip = argv[1];
    }

    Luntik::Game* game = new Luntik::Game(ip);
    int exit_code = game->run();

    std::cout << "Exit code: " << exit_code << std::endl;
    // std::cin.ignore();

    return exit_code;
}
