/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/menu.h
* @author  Calvin Johnson
* @version V1.0.0
* @date    31/10/2019 5:39pm
* @brief   This file declares the variables and functions that are used to
*          handle the menus on the screen
*****************************************************************************/

#ifndef MENU_H_
#define MENU_H_

#include "OLED.h"
#include "settings.h"

#define TOP_BAR_HEIGHT CHAR_HEIGHT + 2

typedef enum MENU_IDS {
    MENU_IN_BASE = 0,
    MENU_IN_MENU = 1,
    MENU_IN_TAB = 2,
    MENU_SCR_SETTINGS = 0,
    MENU_DRV_SELECTION = 1,
    MENU_PWR_SETTINGS = 2,
    MENU_ADV_SETTINGS = 3,
} MENU_IDS;


typedef enum TAB_IDS {
    TAB_SCR_SETTINGS_BRIGHTNESS = 1,
    TAB_SCR_SETTINGS_DASH_INTENSITY = 2,
    TAB_DRV_SELECTION_DRV1 = 1,
    TAB_DRV_SELECTION_DRV2 = 2,
    TAB_DRV_SELECTION_DRV3 = 3,
    TAB_PWR_SETTINGS_SPRINT = 1,
    TAB_PWR_SETTINGS_ENDURANCE = 2,
    TAB_PWR_SETTINGS_LIMP = 3,
    TAB_ADV_SETTINGS_PWR_BIAS = 1,
    TAB_ADV_SETTINGS_REGEN_BRK = 2,
    TAB_ADV_SETTINGS_TORQUE_BRK = 3
} TAB_IDS;

uint8_t menu_max_choices_for_level(uint8_t current_state, uint8_t current_menu);

void menu_handle_screens(Configuration* configuration);

// menus
void menu_handle_navigation(uint16_t* current_screen);
void menu_draw_menu(uint16_t* current_screen);
// tabs

#endif