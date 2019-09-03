/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/main.c
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
