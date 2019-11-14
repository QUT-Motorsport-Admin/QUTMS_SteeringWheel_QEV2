/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/input.c
* @author  Calvin Johnson
* @version V1.0.0
* @date    31/10/2019 6:51:03 PM
* @brief   This file handles input processing
*****************************************************************************/

#include "input.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void configure_input() {
    DDRA = 0b00000000;  // Set buttons as input
    PORTA = 0b11111111; // set the pullups

	PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
	PCMSK0 = (1 << PCINT7) | (1 << PCINT6); // set PCINT7 and 6 to trigger an interrupt on state change
}

volatile uint8_t prevRotA = 0;
volatile bool encoder_changed = false;
volatile int encoder_reading = 0;

input_state previous_state = {0};

ISR(PCINT0_vect) {
	uint8_t rotA = !(BIT_VALUE(PINA, ROT_ENC_A));
	uint8_t rotB = !(BIT_VALUE(PINA, ROT_ENC_B));
	encoder_changed = true;

	// pinA has changed
	if (rotA != prevRotA) {
		
		if (rotA != rotB) {
			// CCW
			encoder_reading = 1;
		} else {
			// CW
			encoder_reading = -1;
		}
	}
	prevRotA = rotA;
}

void read_input(input_state* current_state) {
    *current_state = (input_state) {
        0
    };

    bool left_button = !BIT_VALUE(PINA, BTN_C);
    bool right_button = !BIT_VALUE(PINA, BTN_B);
    bool back_button = !BIT_VALUE(PINA, BTN_A);

	// buttons only true for their first press eg not if held down
    current_state->left_button = (left_button && (previous_state.left_button != left_button));
    current_state->right_button = (right_button && (previous_state.right_button != right_button));
    current_state->back_button = (back_button && (previous_state.back_button != back_button));

	// only read if encoder has moved
	if (encoder_changed) {
		current_state->encoder = encoder_reading;
		encoder_changed = false;
	} else {
		// encoder hasnt moved
		current_state->encoder = 0;
	}

    previous_state.left_button = left_button;
	previous_state.right_button = right_button;
	previous_state.back_button = back_button;
}