/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel/main.c
* @author  Zoe Goodward
* @version V1.0.0
* @date    2/08/2019 1:12:41 PM
* @brief   Functions for creating graphics on the OLED display
*****************************************************************************/

#define F_CPU 16000000UL /* CPU clock in Hertz */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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

int main(void)
{
	while (1)
	{
		LED_A_ON;
		LED_B_OFF;
		_delay_ms(100);		
		LED_A_OFF;
		LED_B_ON;
		_delay_ms(500);
	}
}
