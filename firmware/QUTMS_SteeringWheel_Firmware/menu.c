/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/menu.c
* @author  Calvin Johnson
* @version V1.0.0
* @date    31/10/2019 5:39pm
* @brief   This file declares the variables and functions that are used to
*          handle the menus on the screen
*****************************************************************************/

#include "menu.h"
#include "OLED.h"
#include "input.h"

#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

char* menu_text[4] = {
    "Screen settings",
    "Driver selection",
    "Power mode settings",
    "Advanced settings",
};

char* tab_text[4][4] = {
    {
        "Screen brightness",
        "Dash LED intensity",
    },
    {
        "Isaac Baldry",
        "Austin Burey",
        "Simon Murrel",
    },
    {
        "Sprint mode",
        "Endurance mode",
        "Limp home mode",
    },
    {
        "F/R power bias %",
        "Regen braking %",
        "Torque vectoring",
    },
};


/* current_screen = xyz
    x = {0,1} - base or in menu
    y = {1,2,3,4} - which menu we're in
    z = {1,2,..,n} - which tab we're in
*/
uint16_t current_screen = MENU_IN_BASE;

void menu_handle_screens(Configuration* configuration) {

    menu_handle_navigation(&current_screen);
    uint8_t current_state = current_screen / 100;
    if (current_state != 2) {
        menu_draw_menu(&current_screen);
    } else {
        // custom handling for actual settings
    }
}

uint8_t menu_max_choices_for_level(uint8_t current_state, uint8_t current_menu) {
    if (current_state == 0) {
        return 4;
    } else if (current_state == 1) {
        if (current_menu == MENU_DRV_SELECTION || current_menu == MENU_PWR_SETTINGS || current_menu == MENU_ADV_SETTINGS) {
            return 3;
        } else if (current_menu == MENU_SCR_SETTINGS) {
            return 2;
        }
    }

    return 0;
}

void menu_draw_menu(uint16_t* current_screen) {
    uint8_t current_state = *current_screen / 100;
    uint8_t current_menu = (*current_screen / 10) % 10;
    uint8_t current_selection = (*current_screen) % 10;

    if (current_state == 0) {
        uint8_t length = strlen(menu_text[current_selection]);
        Show_String((OLED_X / 2) - (CHAR_WIDTH * length / 2), OLED_Y / 2, menu_text[current_selection]);
    } else if (current_state == 1) {
        uint8_t length = strlen(tab_text[current_menu][current_selection]);
        Show_String((OLED_X / 2) - (CHAR_WIDTH * length / 2), OLED_Y / 2, tab_text[current_menu][current_selection]);
    }
}

void menu_handle_navigation(uint16_t* current_screen) {
    // for terminal testing, TODO: add actual input
    char c = 0; //get_char();

    // read off previous state
    uint8_t current_state = *current_screen / 100;
    uint8_t current_menu = (*current_screen / 10) % 10;
    uint8_t current_selection = (*current_screen) % 10;
    //uint8_t in_tab = *current_screen % 10;

    bool left_button = !((PINA >> BTN_C) & 1 == 1);
    bool right_button = !((PINA >> BTN_B) & 1 == 1);
    bool back_button = !((PINA >> BTN_A) & 1 == 1);

    if ((left_button == right_button) && left_button == true) {
        if (current_state < 2) {
            if (current_state == 0) {
                current_menu = current_selection;
                current_selection = 0;
            } else {
            }
            current_state += 1;
        }
        Clear_Buffer();
    } else if (back_button) {
        if (current_state > 0) {
            current_state -= 1;

            if (current_state == 0) {
                current_selection = current_menu;
                current_menu = 0;
            }
        }
        Clear_Buffer();
    }
    if (current_state < 2) {
        if (left_button) {
            if (current_selection < menu_max_choices_for_level(current_state, current_menu) - 1) {
                current_selection += 1;
                Clear_Buffer();
            }
        } else if (right_button) {
            if (current_selection > 0) {
                current_selection -= 1;
                Clear_Buffer();
            }
        }
    }

    // set our current state
    *current_screen = current_state * 100 + current_menu * 10 + current_selection;
}