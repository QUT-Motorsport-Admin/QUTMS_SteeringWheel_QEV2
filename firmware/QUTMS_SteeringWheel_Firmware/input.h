/*****************************************************************************
* @file    Software/inc/steeringWheelGraphics.h
* @author  Zoe Goodward
* @version V1.0.0
* @date    28-April-2018
* @brief   This file declares the variables and functions that are used to
*          produce graphics on the OLED screen
*****************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

/*	Pinout

	RX			PD0
	TX			PD1
	
*/

/* SPI */
#define SPI_MOSI	PINB5
#define SPI_MISO	PINB6
#define SPI_SCK		PINB7

/* OLED */
#define OLED_RESET	PINB0
#define OLED_CS		PINB1
#define	OLED_DC		PIND3

/* CAN */
#define CAN_CS		PINB3
#define CAN_INT		PINB4

/* SD_CARD */
#define	SD_CS		PINB2

/* LEDS */
#define LED_A		PIND6
#define LED_B		PIND7

/* Buttons */
#define BTN_A		PINA5
#define BTN_B		PINA6
#define BTN_C		PINA7

#define LED_A_ON	PORTD |= 0b01000000
#define LED_A_OFF	PORTD &= ~0b01000000
#define LED_B_ON	PORTD |= 0b10000000
#define LED_B_OFF	PORTD &= ~0b10000000

#endif /* MAIN_H_ */