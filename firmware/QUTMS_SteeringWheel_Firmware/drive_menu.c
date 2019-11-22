/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/static_menu.c
* @author  Calvin Johnson
* @version V1.0.0
* @date    18/11/2019 12:00pm
* @brief   This file declares the variables and functions that are used to
*          handle the driving menu
*****************************************************************************/

#include "drive_menu.h"
#include "OLED.h"
#include "input.h"
#include <stdbool.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define TIMER0_PRESCALE 8.0
#define TIMER0_FREQ = (F_CPU / TIMER1_PRESCALE)

// variables
uint8_t DM_current_screen = 0;
Canbus_Data canbus_data_storage[DM_CANBUS_TYPE_COUNT];

volatile bool state_changed = false;

double error_timer_count = 0;

volatile uint32_t timer_overflow = 0;

char *DM_screen_headers[DM_CANBUS_TYPE_COUNT] = {
    "Invertors",
    "ERROR",
    // add text for each screen for displaying in function
    // OR ALTERNATIVELY
    // just call your string directly in each display function
};

// interrupts
ISR(TIMER0_OVF_vect) {
    timer_overflow++;
}

// TODO:
// CANbus interrupts
// everytime you get a canbus message set state_changed = true

void DM_init() {
    // setup CANbus

    // setup timer

    TCCR0A = 0;
    TCCR0B = 2;
    // enable overflow interrupt
    TIMSK0 = 1;
}

void DM_process() {
    // read current input state so we can navigate menus
    input_state current_input;
    read_input(&current_input);

    // code to change screen, this is fine as is
    if (current_input.right_button && DM_current_screen < DM_MENU_COUNT - 1) {
        DM_current_screen++;
        state_changed = true;
    } else if (current_input.left_button && DM_current_screen > 0) {
        DM_current_screen--;
        state_changed = true;
    }

    if (state_changed) {
        // we've either changed the current screen or recieved new CANbus data
        Clear_Buffer();

        if (DM_current_screen == CANBUS_TYPE_INVERTOR) {
            DM_display_inverter(&canbus_data_storage[CANBUS_TYPE_INVERTOR]);
        } else if (DM_current_screen == CANBUS_TYPE_ERROR) {
            // call display error function
        }

        // else if etc for each canbus type

        state_changed = false;
    }
}

uint8_t DM_display_inverter(Canbus_Data *CANbus) {
    if (CANbus->length < 6) {
        // bad packet
        return 0;
    }

    uint8_t acceleration_wheel = CANbus->data[0];
    uint8_t brake_pedal = CANbus->data[1];
    uint8_t steering_angle = CANbus->data[2];
    uint8_t is_armed = CANbus->data[3];
    uint8_t siren_count = CANbus->data[4];
    uint8_t general_flags = CANbus->data[5];

    uint8_t contactor_high_on = (general_flags >> 7) & 1;
    uint8_t pre_charge_on = (general_flags >> 6) & 1;
    uint8_t siren_on = (general_flags >> 5) & 1;
    uint8_t brake_light_on = (general_flags >> 4) & 1;
    uint8_t cooling_fan_left_on = (general_flags >> 3) & 1;
    uint8_t cooling_pump_left_on = (general_flags >> 2) & 1;
    uint8_t cooling_fan_right_on = (general_flags >> 1) & 1;
    uint8_t cooling_pump_right_on = (general_flags >> 0) & 1;

    // print stuff to screen
    // Show_String
    // Show_Progress_Bar
    // etc
}

uint8_t DM_display_wheel(Canbus_Data *CANbus) {
    // print stuff to screen
    // Show_String
    // Show_Progress_Bar
    // etc
}

uint8_t DM_display_PDM(Canbus_Data *CANbus) {
    if (CANbus->length < 6) {
        // bad packet
        return 0;
    }

    uint8_t general_flags = CANbus->data[0];

    uint8_t contactor_high_on = (general_flags >> 7) & 1;
    uint8_t pre_charge_on = (general_flags >> 6) & 1;
    uint8_t siren_on = (general_flags >> 5) & 1;
    uint8_t brake_light_on = (general_flags >> 4) & 1;
    uint8_t cooling_fan_left_on = (general_flags >> 3) & 1;
    uint8_t cooling_pump_left_on = (general_flags >> 2) & 1;
    uint8_t cooling_fan_right_on = (general_flags >> 1) & 1;
    uint8_t cooling_pump_right_on = (general_flags >> 0) & 1;

    // print stuff to screen
    // Show_String
    // Show_Progress_Bar
    // etc
}

uint8_t DM_process_error(Canbus_Data *error) {
    /*
        if error stays until button press just display it, and if we get a button press go back to other screen or something
        if error stays on timer use get_current_time() to see if enough time has passed for it to vanish
    */
}

double get_current_time() {
    return (timer_overflow * 256.0 + TCNT0) * (TIMER0_PRESCALE / F_CPU);
}