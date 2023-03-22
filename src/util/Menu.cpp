//
// Created by Alexander Rossiter on 17/08/2022.
//

#include "Menu.h"
#include <iostream>

void
Menu::starting_menu()
{
    std::cout << header << std::endl;

    std::cout << "Starting options: \n(1) Run test case.\n(2) Run own case." << std::endl;

    int  usr_in        = -1;
    bool continue_loop = true;

    while (continue_loop) {

        std::cin >> usr_in;
        std::cout << std::endl;

        switch (usr_in) {
            case (1):
                test_case_menu();
                initial_guess_menu();
                continue_loop = false;
                break;
            case (2):
                own_case_menu();
                initial_guess_menu();
                continue_loop = false;
                break;
            default:
                break;
        }
    }
}

void
Menu::test_case_menu()
{
    std::cout << "Test cases: \n(1) Gaussian bump.\n(2) naca-2412.\n(3) "
                 "Wedge\n(4) Nash"
              << std::endl;

    int usr_in;
    std::cin >> usr_in;
    std::cout << std::endl;

    switch (usr_in) {
        case (1):
            case_name = "gaussian_bump";
            break;
        case (2):
            case_name = "naca-2412";
            break;
        case (3):
            case_name = "wedge";
            break;
        case (4):
            case_name = "Nash_Rig";
            break;
        default:
            break;
    }
    directory = "../resources/test_cases/";
}

void
Menu::own_case_menu()
{
    std::cout << "Put your .grid and .patch file in the working "
                 "directory.\nEnter case name...";
    directory = "../";
    std::cin >> case_name;
}

void
Menu::initial_guess_menu()
{
    std::string usr_in;
    std::cout << "Use own guess? y/n" << std::endl;
    std::cin >> usr_in;

    if (usr_in == "y") {
        std::cout << "Enter Mach number guess..." << std::endl;
        std::cin >> ig_M;
        std::cout << "Enter Po guess..." << std::endl;
        std::cin >> ig_Po;
        std::cout << "Enter To guess..." << std::endl;
        std::cin >> ig_To;
        std::cout << "Enter yaw guess..." << std::endl;
        std::cin >> ig_yaw;
        std::cout << "Enter pitch guess..." << std::endl;
        std::cin >> ig_pitch;
    }
}

Menu::Menu() {}

Menu::~Menu() {}
