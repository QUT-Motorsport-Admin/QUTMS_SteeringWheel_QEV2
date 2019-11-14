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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
        "Sprint",
        "Endurance",
        "Limp home",
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
input_state current_input = {0};
//input_state previous_input = {0};

void menu_handle_screens(Configuration** configuration) {
	// update input
	read_input(&current_input);
    // draw current driver and power
    // draw current driver
    Show_Formatted(2, 1, "Driver: %s", tab_text[MENU_DRV_SELECTION][(*configuration)->driver_index]);
    Show_Formatted(OLED_X / 2, 1, "Power Mode: %s", tab_text[MENU_PWR_SETTINGS][(*configuration)->power_mode_index]);

    menu_handle_navigation(&current_screen);
    uint8_t current_state = current_screen / 100;
    if (current_state != 2) {
        menu_draw_menu(&current_screen);
    } else {
        uint8_t current_menu = (current_screen / 10) % 10;
        uint8_t current_selection = (current_screen) % 10;

        if (current_menu == MENU_SCR_SETTINGS) {
            if (current_selection == TAB_SCR_SETTINGS_BRIGHTNESS) {
                adjust_value_tab(&(*configuration)->screen_brightness, tab_text[MENU_SCR_SETTINGS][TAB_SCR_SETTINGS_BRIGHTNESS], &current_screen);
            } else if (current_selection == TAB_SCR_SETTINGS_DASH_INTENSITY) {
                adjust_value_tab(&(*configuration)->led_intensity, tab_text[MENU_SCR_SETTINGS][TAB_SCR_SETTINGS_DASH_INTENSITY], &current_screen);
            }

        } else if (current_menu == MENU_DRV_SELECTION) {
            *configuration = get_driver_config(current_selection);

        } else if (current_menu == MENU_PWR_SETTINGS) {
            (*configuration)->power_mode_index = current_selection;

        } else if (current_menu == MENU_ADV_SETTINGS) {
            if (current_selection == TAB_ADV_SETTINGS_PWR_BIAS) {
                adjust_value_tab(&(*configuration)->power_bias, tab_text[MENU_ADV_SETTINGS][TAB_ADV_SETTINGS_PWR_BIAS], &current_screen);

            } else if (current_selection == TAB_ADV_SETTINGS_REGEN_BRK) {
                adjust_value_tab(&(*configuration)->regen_braking, tab_text[MENU_ADV_SETTINGS][TAB_ADV_SETTINGS_REGEN_BRK], &current_screen);

            } else if (current_selection == TAB_ADV_SETTINGS_TORQUE_BRK) {
            }
        }
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
    //char c = 0; //get_char();

    // read off previous state
    uint8_t current_state = *current_screen / 100;
    uint8_t current_menu = (*current_screen / 10) % 10;
    uint8_t current_selection = (*current_screen) % 10;
    //uint8_t in_tab = *current_screen % 10;

    

    /*bool left_button = current_input.left_button && !previous_input.left_button;
    bool right_button = current_input.right_button && !previous_input.right_button;
    bool back_button = current_input.back_button && !previous_input.back_button;*/

   // previous_input = current_input;
   if (current_input.back_button){
    //if ((current_input.left_button == current_input.right_button) && current_input.left_button == true) {
        if (current_state < 2) {
            if (current_state == 0) {
                current_menu = current_selection;
                current_selection = 0;
            } else {
            }
            current_state += 1;
        }
        Clear_Buffer();
    }/* else if (current_input.back_button) {
        menu_exit_tab(current_screen);
        current_state = *current_screen / 100;
        current_menu = (*current_screen / 10) % 10;
        current_selection = (*current_screen) % 10;
        Clear_Buffer();
    }*/
    if (current_state < 2) {
        if (current_input.left_button) {
            if (current_selection < menu_max_choices_for_level(current_state, current_menu) - 1) {
                current_selection += 1;
                Clear_Buffer();
            }
        } else if (current_input.right_button) {
            if (current_selection > 0) {
                current_selection -= 1;
                Clear_Buffer();
            }
        }
    }

    // set our current state
    *current_screen = current_state * 100 + current_menu * 10 + current_selection;
}

void menu_exit_tab(uint16_t* current_screen) {
    uint8_t current_state = *current_screen / 100;
    uint8_t current_menu = (*current_screen / 10) % 10;
    uint8_t current_selection = (*current_screen) % 10;
    if (current_state > 0) {
        current_state -= 1;

        if (current_state == 0) {
            current_selection = current_menu;
            current_menu = 0;
        }
    }
    *current_screen = current_state * 100 + current_menu * 10 + current_selection;
}

Configuration* get_driver_config(uint8_t driver_index) {
    // TODO: make this read from the EPROM
    return NULL;
}

int temp_adjustment = -1;
void adjust_value_tab(int* value, char* title, uint16_t* current_screen) {
    if (temp_adjustment < 0) {
        temp_adjustment = *value;
    }
    int titleLength = strlen(title);
    int settingWidth = OLED_X / 2;
    char* prevText = "Previous Setting";
    char* newText = "New setting";

    Show_String(OLED_X / 2 - (titleLength / 2)*CHAR_WIDTH, TOP_BAR_HEIGHT, title);

    Show_String((settingWidth / 2) - (strlen(prevText) / 2) * CHAR_WIDTH, TOP_BAR_HEIGHT + CHAR_HEIGHT, prevText);
    Show_Formatted((settingWidth / 2) - 3, TOP_BAR_HEIGHT + 2*CHAR_HEIGHT, "%03d %c", *value, '%');

    Show_String(1.5 * settingWidth - (strlen(newText) / 2) * CHAR_WIDTH, TOP_BAR_HEIGHT + CHAR_HEIGHT, newText);
    Show_Formatted(1.5 * settingWidth - 3 * CHAR_WIDTH, TOP_BAR_HEIGHT + 2*CHAR_HEIGHT, "%03d %c", temp_adjustment, '%');

	temp_adjustment += current_input.encoder * 2;

    if (temp_adjustment < 0) {
		temp_adjustment = 0;
    }
    if (temp_adjustment > 100) {
		temp_adjustment = 100;
    }
    if (current_input.select_button) {
        *value = temp_adjustment;
        temp_adjustment = -1;
        menu_exit_tab(current_screen);
    } else if (current_input.back_button) {
        temp_adjustment = -1;
    }
}