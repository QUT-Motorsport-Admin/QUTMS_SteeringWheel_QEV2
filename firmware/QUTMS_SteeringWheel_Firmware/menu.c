#include "menu.h"
#include "OLED.h"
#include <stdint.h>
#include <string.h>

/* current_screen = xyz
    x = {0,1} - base or in menu
    y = {1,2,3,4} - which menu we're in
    z = {1,2,..,n} - which tab we're in
*/
uint8_t current_screen = MENU_IN_BASE;

void menu_handle_screens(Configuration *configuration) {

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

void menu_draw_menu(uint16_t *current_screen) {
}

void menu_handle_navigation(uint16_t *current_screen) {
    // for terminal testing, TODO: add actual input
    char c = 0; //get_char();

    // read off previous state
    uint8_t current_state = *current_screen / 100;
    uint8_t current_menu = (*current_screen / 10) % 10;
    uint8_t current_selection = (*current_screen) % 10;
    //uint8_t in_tab = *current_screen % 10;

    if (c == 't') {
        if (current_state < 2) {
            if (current_state == 0) {
                current_menu = current_selection;
                current_selection = 0;
            } else {
            }
            current_state += 1;
        }
    } else if (c == 'q') {
        if (current_state > 0) {
            current_state -= 1;

            if (current_state == 0) {
                current_selection = current_menu;
                current_menu = 0;
            }
        }
    }
    if (current_state < 2) {
        if (c == 's') {
            if (current_selection < menu_max_choices_for_level(current_state, current_menu) - 1) {
                current_selection += 1;
            }
        } else if (c == 'w') {
            if (current_selection > 0) {
                current_selection -= 1;
            }
        }
    }

    // set our current state
    *current_screen = current_state * 100 + current_menu * 10 + current_selection;
}