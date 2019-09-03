/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/OLED.c
* @author  Zoe Goodward
* @version V1.0.0
* @date    3/08/2019 12:43:47 PM
* @brief   This file...
*****************************************************************************/

/* OLED INFO 
* Screen size: 256*64
* Driver IC: SSD1322 (Solomon Systech)
* Interface: 4-wire SPI
*/

#define F_CPU 16000000UL /* CPU clock in Hertz */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "OLED.h"
#include "spi.h"

void oled_write_data(uint8_t data)
{
	OLED_CS_LOW;
	OLED_DC_DATA;
	spi_send_byte(data);
	OLED_DC_DATA;
	OLED_CS_HIGH;
}

void oled_write_instruction(uint8_t data)
{
	OLED_DC_INSTRUCTION;
	OLED_CS_LOW;
	spi_send_byte(data);
	OLED_DC_DATA;
	OLED_CS_HIGH;
}

/*============================================================================
Function:   oled_init()
------------------------------------------------------------------------------
Purpose :   initialises the OLED screen
Input   :   none
Returns :   void
Notes   :
============================================================================*/
void OLED_init(void)
{
	OLED_RESET_LOW;
	//Delay(1000);
	_delay_ms(1000);
	OLED_RESET_HIGH;
	
	set_command_lock(0x12);			// Unlock Basic Commands (0x12/0x16)
	set_display_on_off(0x00);		// Display Off (0x00/0x01)
	set_display_clock(0x91);		// Set Clock as 80 Frames/Sec
	set_multiplex_ratio(0x3F);		// 1/64 Duty (0x0F~0x3F)
	set_display_offset(0x00);		// Shift Mapping RAM Counter (0x00~0x3F)
	set_start_line(0x00);			// Set Mapping RAM Display Start Line (0x00~0x7F)
	set_remap_format(0x14);			// Set Horizontal Address Increment
	//     Column Address 0 Mapped to SEG0
	//     Disable Nibble Remap
	//     Scan from COM[N-1] to COM0
	//     Disable COM Split Odd Even
	//     Enable Dual COM Line Mode
	set_GPIO(0x00);							// Disable GPIO Pins Input
	set_function_selection(0x01);			// Enable Internal VDD Regulator
	set_display_enhancement_A(0xA0,0xFD);	// Enable External VSL
	// Set Low Gray Scale Enhancement
	set_contrast_current(0x7F);				// Set Segment Output Current
	set_master_current(BRIGHTNESS);			// Set Scale Factor of Segment Output Current Control
	set_gray_scale_table();					// Set Pulse Width for Gray Scale Table
	set_phase_length(0xE2);					// Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
	set_display_enhancement_B(0x20);		// Enhance Driving Scheme Capability (0x00/0x20)
	set_precharge_voltage(0x1F);			// Set Pre-Charge Voltage Level as 0.60*VCC
	set_precharge_period(0x08);				// Set Second Pre-Charge Period as 8 Clocks
	set_VCOMH(0x07);						// Set Common Pins Deselect Voltage Level as 0.86*VCC
	set_display_mode(0x02);					// Normal Display Mode (0x00/0x01/0x02/0x03)
	set_partial_display(0x01,0x00,0x00);	// Disable Partial Display
	
	fill_RAM(CLEAR_SCREEN);					// Clear Screen
	
	set_display_on_off(0x01);				// Display On (0x00/0x01)
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void set_column_address(uint8_t a, uint8_t b)
{
	oled_write_instruction(0x15);			// Set Column Address
	oled_write_data(a);						//   Default => 0x00
	oled_write_data(b);						//   Default => 0x77
}


void set_row_address(uint8_t a, uint8_t b)
{
	oled_write_instruction(0x75);			// Set Row Address
	oled_write_data(a);				//   Default => 0x00
	oled_write_data(b);				//   Default => 0x7F
}


void set_write_RAM()
{
	oled_write_instruction(0x5C);			// Enable MCU to Write into RAM
}


void set_read_RAM()
{
	oled_write_instruction(0x5D);			// Enable MCU to Read from RAM
}


void set_remap_format(uint8_t d)
{
	oled_write_instruction(0xA0);			// Set Re-Map / Dual COM Line Mode
	oled_write_data(d);				//   Default => 0x40
	//     Horizontal Address Increment
	//     Column Address 0 Mapped to SEG0
	//     Disable Nibble Remap
	//     Scan from COM0 to COM[N-1]
	//     Disable COM Split Odd Even
	oled_write_data(0x11);			//   Default => 0x01 (Disable Dual COM Mode)
}


void set_start_line(uint8_t d)
{
	oled_write_instruction(0xA1);			// Set Vertical Scroll by RAM
	oled_write_data(d);				//   Default => 0x00
}


void set_display_offset(uint8_t d)
{
	oled_write_instruction(0xA2);			// Set Vertical Scroll by Row
	oled_write_data(d);				//   Default => 0x00
}


void set_display_mode(uint8_t d)
{
	oled_write_instruction(0xA4|d);			// Set Display Mode
	//   Default => 0xA4
	//     0xA4 (0x00) => Entire Display Off, All Pixels Turn Off
	//     0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 15
	//     0xA6 (0x02) => Normal Display
	//     0xA7 (0x03) => Inverse Display
}


void set_partial_display(uint8_t a, uint8_t b, uint8_t c)
{
	oled_write_instruction(0xA8|a);
	// Default => 0x8F
	//   Select Internal Booster at Display On
	if(a == 0x00)
	{
		oled_write_data(b);
		oled_write_data(c);
	}
}


void set_function_selection(uint8_t d)
{
	oled_write_instruction(0xAB);			// Function Selection
	oled_write_data(d);				//   Default => 0x01
	//     Enable Internal VDD Regulator
}


void set_display_on_off(uint8_t d)
{
	oled_write_instruction(0xAE|d);			// Set Display On/Off
	//   Default => 0xAE
	//     0xAE (0x00) => Display Off (Sleep Mode On)
	//     0xAF (0x01) => Display On (Sleep Mode Off)
}


void set_phase_length(uint8_t d)
{
	oled_write_instruction(0xB1);			// Phase 1 (Reset) & Phase 2 (Pre-Charge) Period Adjustment
	oled_write_data(d);				//   Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])
	//     D[3:0] => Phase 1 Period in 5~31 Display Clocks
	//     D[7:4] => Phase 2 Period in 3~15 Display Clocks
}


void set_display_clock(uint8_t d)
{
	oled_write_instruction(0xB3);			// Set Display Clock Divider / Oscillator Frequency
	oled_write_data(d);				//   Default => 0xD0
	//     A[3:0] => Display Clock Divider
	//     A[7:4] => Oscillator Frequency
}


void set_display_enhancement_A(uint8_t a, uint8_t b)
{
	oled_write_instruction(0xB4);			// Display Enhancement
	oled_write_data(0xA0|a);			//   Default => 0xA2
	//     0xA0 (0x00) => Enable External VSL
	//     0xA2 (0x02) => Enable Internal VSL (Kept VSL Pin N.C.)
	oled_write_data(0x05|b);			//   Default => 0xB5
	//     0xB5 (0xB0) => Normal
	//     0xFD (0xF8) => Enhance Low Gray Scale Display Quality
}


void set_GPIO(uint8_t d)
{
	oled_write_instruction(0xB5);			// General Purpose IO
	oled_write_data(d);				//   Default => 0x0A (GPIO Pins output Low Level.)
}


void set_precharge_period(uint8_t d)
{
	oled_write_instruction(0xB6);			// Set Second Pre-Charge Period
	oled_write_data(d);				//   Default => 0x08 (8 Display Clocks)
}


void set_precharge_voltage(uint8_t d)
{
	oled_write_instruction(0xBB);			// Set Pre-Charge Voltage Level
	oled_write_data(d);				//   Default => 0x17 (0.50*VCC)
}


void set_VCOMH(uint8_t d)
{
	oled_write_instruction(0xBE);			// Set COM Deselect Voltage Level
	oled_write_data(d);				//   Default => 0x04 (0.80*VCC)
}


void set_contrast_current(uint8_t d)
{
	oled_write_instruction(0xC1);			// Set Contrast Current
	oled_write_data(d);				//   Default => 0x7F
}


void set_master_current(uint8_t d)
{
	oled_write_instruction(0xC7);			// Master Contrast Current Control
	oled_write_data(d);				//   Default => 0x0f (Maximum)
}


void set_multiplex_ratio(uint8_t d)
{
	oled_write_instruction(0xCA);			// Set Multiplex Ratio
	oled_write_data(d);				//   Default => 0x7F (1/128 Duty)
}


void set_display_enhancement_B(uint8_t d)
{
	oled_write_instruction(0xD1);			// Display Enhancement
	oled_write_data(0x82|d);			//   Default => 0xA2
	//     0x82 (0x00) => Reserved
	//     0xA2 (0x20) => Normal
	oled_write_data(0x20);
}


void set_command_lock(uint8_t d)
{
	oled_write_instruction(0xFD);			// Set Command Lock
	oled_write_data(0x12|d);			//   Default => 0x12
	//     0x12 => Driver IC interface is unlocked from entering command.
	//     0x16 => All Commands are locked except 0xFD.
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void fill_RAM(uint8_t Data)
{
	uint8_t i,j;

	set_column_address(0x00,0x77);
	set_row_address(0x00,0x7F);
	set_write_RAM();

	for(i=0;i<128;i++)
	{
		for(j=0;j<120;j++)
		{
			oled_write_data(Data);
			//Write_Data(Data);
		}
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void set_gray_scale_table()
{
	oled_write_instruction(0xB8);			// Set Gray Scale Table
	oled_write_data(0x0C);			//   Gray Scale Level 1
	oled_write_data(0x18);			//   Gray Scale Level 2
	oled_write_data(0x24);			//   Gray Scale Level 3
	oled_write_data(0x30);			//   Gray Scale Level 4
	oled_write_data(0x3C);			//   Gray Scale Level 5
	oled_write_data(0x48);			//   Gray Scale Level 6
	oled_write_data(0x54);			//   Gray Scale Level 7
	oled_write_data(0x60);			//   Gray Scale Level 8
	oled_write_data(0x6C);			//   Gray Scale Level 9
	oled_write_data(0x78);			//   Gray Scale Level 10
	oled_write_data(0x84);			//   Gray Scale Level 11
	oled_write_data(0x90);			//   Gray Scale Level 12
	oled_write_data(0x9C);			//   Gray Scale Level 13
	oled_write_data(0xA8);			//   Gray Scale Level 14
	oled_write_data(0xB4);			//   Gray Scale Level 15

	oled_write_instruction(0x00);			// Enable Gray Scale Table
}