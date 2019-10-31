/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/input.c
* @author  Calvin Johnson
* @version V1.0.0
* @date    31/10/2019 6:51:03 PM
* @brief   This file handles input processing
*****************************************************************************/

#include "input.h"
#include <avr/io.h>

//input_state previous_state = {0};

void read_input(input_state* current_state) {
    *current_state = (input_state) {
        0
    };

    bool left_button = !((PINA >> BTN_C) & 1 == 1);
    bool right_button = !((PINA >> BTN_B) & 1 == 1);
    bool back_button = !((PINA >> BTN_A) & 1 == 1);

    current_state->left_button = left_button;//(left_button && (previous_state.left_button != left_button));
    current_state->right_button = right_button; //(right_button && (previous_state.right_button != right_button));
    current_state->back_button = back_button;//(back_button && (previous_state.back_button != back_button));

    //previous_state = *current_state;
}