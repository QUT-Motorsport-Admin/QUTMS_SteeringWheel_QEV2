/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/drive_menu.h
* @author  Calvin Johnson
* @version V1.0.0
* @date    18/11/2019 12:00pm
* @brief   This file declares the variables and functions that are used to
*          handle the driving menu
*****************************************************************************/

#ifndef DRIVE_MENU_H_
#define DRIVE_MENU_H_

#include <stdint.h>

// DEFINES
#define ERROR_DISPLAY_TIME 3.0
#define DM_MENU_COUNT 4
#define DM_CANBUS_TYPE_COUNT 5

// ENUMS

typedef enum CANBUS_TYPE {
    CANBUS_TYPE_INVERTOR = 0,
    CANBUS_TYPE_ERROR = 1
} CANBUS_TYPE;

// STRUCTS

typedef struct Canbus_Data {
    uint8_t data[8];
    uint8_t length;
    uint8_t type;
} Canbus_Data;

// GLOBALS

// METHODS

// setup everything requried including CANbus inputs
void DM_init();

/*
    main loop for drive menu, handles menu state
    and displays information
 */
void DM_process();

uint8_t DM_display_inverter(Canbus_Data *CANbus);
uint8_t DM_display_wheel(Canbus_Data *CANbus);
uint8_t DM_display_PDM(Canbus_Data *CANbus);

/*
    if we recieved an error from CANbus, either block the screen for 
*/
uint8_t DM_process_error(Canbus_Data *error);

// simple interrupt timer
double get_current_time();

#endif