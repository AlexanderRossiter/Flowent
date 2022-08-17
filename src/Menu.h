//
// Created by Alexander Rossiter on 17/08/2022.
//

#ifndef FLOWENT_MENU_H
#define FLOWENT_MENU_H


#include <string>

class Menu {
public:
    float ig_M = 0.6;
    float ig_Po = 48289.;
    float ig_To = 300;
    float ig_yaw = 0;
    float ig_pitch = 0;
    std::string case_name;
    std::string directory;
    std::string gas_string = "air";

    Menu();

    void starting_menu();

    virtual ~Menu();


private:
    void test_case_menu();
    void own_case_menu();
    void initial_guess_menu();





    std::string header = R"(
    oooooooooooo ooooo          .oooooo.   oooooo   oooooo     oooo oooooooooooo ooooo      ooo ooooooooooooo
    `888'     `8 `888'         d8P'  `Y8b   `888.    `888.     .8'  `888'     `8 `888b.     `8' 8'   888   `8
    888          888         888      888   `888.   .8888.   .8'    888          8 `88b.    8       888
    888oooo8     888         888      888    `888  .8'`888. .8'     888oooo8     8   `88b.  8       888
    888    "     888         888      888     `888.8'  `888.8'      888    "     8     `88b.8       888
    888          888       o `88b    d88'      `888'    `888'       888       o  8       `888       888
    o888o        o888ooooood8  `Y8bood8P'        `8'      `8'       o888ooooood8 o8o        `8      o888o
    )";

};


#endif //FLOWENT_MENU_H
