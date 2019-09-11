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
			//oled_write_data(Data);
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

/////////////////////////////////////
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show String
//
//    a: Database
//    b: Start X Address
//    c: Start Y Address
//    * Must write "0" in the end...
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_String(unsigned char a, unsigned char *Data_Pointer, unsigned char b, unsigned char c)
{
	unsigned char *Src_Pointer;

	Src_Pointer=Data_Pointer;
	Show_Font57(1,96,b,c);			// No-Break Space
	//   Must be written first before the string start...

	while(1)
	{
		Show_Font57(a,*Src_Pointer,b,c);
		Src_Pointer++;
		b+=2;
		if(*Src_Pointer == 0) break;
	}
}

unsigned char Ascii_2[240][5]  ={		// Refer to "Times New Roman" Font Database...
	//   Basic Characters
	{0x00,0x00,0x4F,0x00,0x00},		//   (  1)  ! - 0x0021 Exclamation Mark
	{0x00,0x07,0x00,0x07,0x00},		//   (  2)  " - 0x0022 Quotation Mark
	{0x14,0x7F,0x14,0x7F,0x14},		//   (  3)  # - 0x0023 Number Sign
	{0x24,0x2A,0x7F,0x2A,0x12},		//   (  4)  $ - 0x0024 Dollar Sign
	{0x23,0x13,0x08,0x64,0x62},		//   (  5)  % - 0x0025 Percent Sign
	{0x36,0x49,0x55,0x22,0x50},		//   (  6)  & - 0x0026 Ampersand
	{0x00,0x05,0x03,0x00,0x00},		//   (  7)  ' - 0x0027 Apostrophe
	{0x00,0x1C,0x22,0x41,0x00},		//   (  8)  ( - 0x0028 Left Parenthesis
	{0x00,0x41,0x22,0x1C,0x00},		//   (  9)  ) - 0x0029 Right Parenthesis
	{0x14,0x08,0x3E,0x08,0x14},		//   ( 10)  * - 0x002A Asterisk
	{0x08,0x08,0x3E,0x08,0x08},		//   ( 11)  + - 0x002B Plus Sign
	{0x00,0x50,0x30,0x00,0x00},		//   ( 12)  , - 0x002C Comma
	{0x08,0x08,0x08,0x08,0x08},		//   ( 13)  - - 0x002D Hyphen-Minus
	{0x00,0x60,0x60,0x00,0x00},		//   ( 14)  . - 0x002E Full Stop
	{0x20,0x10,0x08,0x04,0x02},		//   ( 15)  / - 0x002F Solidus
	{0x3E,0x51,0x49,0x45,0x3E},		//   ( 16)  0 - 0x0030 Digit Zero
	{0x00,0x42,0x7F,0x40,0x00},		//   ( 17)  1 - 0x0031 Digit One
	{0x42,0x61,0x51,0x49,0x46},		//   ( 18)  2 - 0x0032 Digit Two
	{0x21,0x41,0x45,0x4B,0x31},		//   ( 19)  3 - 0x0033 Digit Three
	{0x18,0x14,0x12,0x7F,0x10},		//   ( 20)  4 - 0x0034 Digit Four
	{0x27,0x45,0x45,0x45,0x39},		//   ( 21)  5 - 0x0035 Digit Five
	{0x3C,0x4A,0x49,0x49,0x30},		//   ( 22)  6 - 0x0036 Digit Six
	{0x01,0x71,0x09,0x05,0x03},		//   ( 23)  7 - 0x0037 Digit Seven
	{0x36,0x49,0x49,0x49,0x36},		//   ( 24)  8 - 0x0038 Digit Eight
	{0x06,0x49,0x49,0x29,0x1E},		//   ( 25)  9 - 0x0039 Digit Nine
	{0x00,0x36,0x36,0x00,0x00},		//   ( 26)  : - 0x003A Colon
	{0x00,0x56,0x36,0x00,0x00},		//   ( 27)  ; - 0x003B Semicolon
	{0x08,0x14,0x22,0x41,0x00},		//   ( 28)  < - 0x003C Less-Than Sign
	{0x14,0x14,0x14,0x14,0x14},		//   ( 29)  = - 0x003D Equals Sign
	{0x00,0x41,0x22,0x14,0x08},		//   ( 30)  > - 0x003E Greater-Than Sign
	{0x02,0x01,0x51,0x09,0x06},		//   ( 31)  ? - 0x003F Question Mark
	{0x32,0x49,0x79,0x41,0x3E},		//   ( 32)  @ - 0x0040 Commercial At
	{0x7E,0x11,0x11,0x11,0x7E},		//   ( 33)  A - 0x0041 Latin Capital Letter A
	{0x7F,0x49,0x49,0x49,0x36},		//   ( 34)  B - 0x0042 Latin Capital Letter B
	{0x3E,0x41,0x41,0x41,0x22},		//   ( 35)  C - 0x0043 Latin Capital Letter C
	{0x7F,0x41,0x41,0x22,0x1C},		//   ( 36)  D - 0x0044 Latin Capital Letter D
	{0x7F,0x49,0x49,0x49,0x41},		//   ( 37)  E - 0x0045 Latin Capital Letter E
	{0x7F,0x09,0x09,0x09,0x01},		//   ( 38)  F - 0x0046 Latin Capital Letter F
	{0x3E,0x41,0x49,0x49,0x7A},		//   ( 39)  G - 0x0047 Latin Capital Letter G
	{0x7F,0x08,0x08,0x08,0x7F},		//   ( 40)  H - 0x0048 Latin Capital Letter H
	{0x00,0x41,0x7F,0x41,0x00},		//   ( 41)  I - 0x0049 Latin Capital Letter I
	{0x20,0x40,0x41,0x3F,0x01},		//   ( 42)  J - 0x004A Latin Capital Letter J
	{0x7F,0x08,0x14,0x22,0x41},		//   ( 43)  K - 0x004B Latin Capital Letter K
	{0x7F,0x40,0x40,0x40,0x40},		//   ( 44)  L - 0x004C Latin Capital Letter L
	{0x7F,0x02,0x0C,0x02,0x7F},		//   ( 45)  M - 0x004D Latin Capital Letter M
	{0x7F,0x04,0x08,0x10,0x7F},		//   ( 46)  N - 0x004E Latin Capital Letter N
	{0x3E,0x41,0x41,0x41,0x3E},		//   ( 47)  O - 0x004F Latin Capital Letter O
	{0x7F,0x09,0x09,0x09,0x06},		//   ( 48)  P - 0x0050 Latin Capital Letter P
	{0x3E,0x41,0x51,0x21,0x5E},		//   ( 49)  Q - 0x0051 Latin Capital Letter Q
	{0x7F,0x09,0x19,0x29,0x46},		//   ( 50)  R - 0x0052 Latin Capital Letter R
	{0x46,0x49,0x49,0x49,0x31},		//   ( 51)  S - 0x0053 Latin Capital Letter S
	{0x01,0x01,0x7F,0x01,0x01},		//   ( 52)  T - 0x0054 Latin Capital Letter T
	{0x3F,0x40,0x40,0x40,0x3F},		//   ( 53)  U - 0x0055 Latin Capital Letter U
	{0x1F,0x20,0x40,0x20,0x1F},		//   ( 54)  V - 0x0056 Latin Capital Letter V
	{0x3F,0x40,0x38,0x40,0x3F},		//   ( 55)  W - 0x0057 Latin Capital Letter W
	{0x63,0x14,0x08,0x14,0x63},		//   ( 56)  X - 0x0058 Latin Capital Letter X
	{0x07,0x08,0x70,0x08,0x07},		//   ( 57)  Y - 0x0059 Latin Capital Letter Y
	{0x61,0x51,0x49,0x45,0x43},		//   ( 58)  Z - 0x005A Latin Capital Letter Z
	{0x00,0x7F,0x41,0x41,0x00},		//   ( 59)  [ - 0x005B Left Square Bracket
	{0x02,0x04,0x08,0x10,0x20},		//   ( 60)  \ - 0x005C Reverse Solidus
	{0x00,0x41,0x41,0x7F,0x00},		//   ( 61)  ] - 0x005D Right Square Bracket
	{0x04,0x02,0x01,0x02,0x04},		//   ( 62)  ^ - 0x005E Circumflex Accent
	{0x40,0x40,0x40,0x40,0x40},		//   ( 63)  _ - 0x005F Low Line
	{0x01,0x02,0x04,0x00,0x00},		//   ( 64)  ` - 0x0060 Grave Accent
	{0x20,0x54,0x54,0x54,0x78},		//   ( 65)  a - 0x0061 Latin Small Letter A
	{0x7F,0x48,0x44,0x44,0x38},		//   ( 66)  b - 0x0062 Latin Small Letter B
	{0x38,0x44,0x44,0x44,0x20},		//   ( 67)  c - 0x0063 Latin Small Letter C
	{0x38,0x44,0x44,0x48,0x7F},		//   ( 68)  d - 0x0064 Latin Small Letter D
	{0x38,0x54,0x54,0x54,0x18},		//   ( 69)  e - 0x0065 Latin Small Letter E
	{0x08,0x7E,0x09,0x01,0x02},		//   ( 70)  f - 0x0066 Latin Small Letter F
	{0x06,0x49,0x49,0x49,0x3F},		//   ( 71)  g - 0x0067 Latin Small Letter G
	{0x7F,0x08,0x04,0x04,0x78},		//   ( 72)  h - 0x0068 Latin Small Letter H
	{0x00,0x44,0x7D,0x40,0x00},		//   ( 73)  i - 0x0069 Latin Small Letter I
	{0x20,0x40,0x44,0x3D,0x00},		//   ( 74)  j - 0x006A Latin Small Letter J
	{0x7F,0x10,0x28,0x44,0x00},		//   ( 75)  k - 0x006B Latin Small Letter K
	{0x00,0x41,0x7F,0x40,0x00},		//   ( 76)  l - 0x006C Latin Small Letter L
	{0x7C,0x04,0x18,0x04,0x7C},		//   ( 77)  m - 0x006D Latin Small Letter M
	{0x7C,0x08,0x04,0x04,0x78},		//   ( 78)  n - 0x006E Latin Small Letter N
	{0x38,0x44,0x44,0x44,0x38},		//   ( 79)  o - 0x006F Latin Small Letter O
	{0x7C,0x14,0x14,0x14,0x08},		//   ( 80)  p - 0x0070 Latin Small Letter P
	{0x08,0x14,0x14,0x18,0x7C},		//   ( 81)  q - 0x0071 Latin Small Letter Q
	{0x7C,0x08,0x04,0x04,0x08},		//   ( 82)  r - 0x0072 Latin Small Letter R
	{0x48,0x54,0x54,0x54,0x20},		//   ( 83)  s - 0x0073 Latin Small Letter S
	{0x04,0x3F,0x44,0x40,0x20},		//   ( 84)  t - 0x0074 Latin Small Letter T
	{0x3C,0x40,0x40,0x20,0x7C},		//   ( 85)  u - 0x0075 Latin Small Letter U
	{0x1C,0x20,0x40,0x20,0x1C},		//   ( 86)  v - 0x0076 Latin Small Letter V
	{0x3C,0x40,0x30,0x40,0x3C},		//   ( 87)  w - 0x0077 Latin Small Letter W
	{0x44,0x28,0x10,0x28,0x44},		//   ( 88)  x - 0x0078 Latin Small Letter X
	{0x0C,0x50,0x50,0x50,0x3C},		//   ( 89)  y - 0x0079 Latin Small Letter Y
	{0x44,0x64,0x54,0x4C,0x44},		//   ( 90)  z - 0x007A Latin Small Letter Z
	{0x00,0x08,0x36,0x41,0x00},		//   ( 91)  { - 0x007B Left Curly Bracket
	{0x00,0x00,0x7F,0x00,0x00},		//   ( 92)  | - 0x007C Vertical Line
	{0x00,0x41,0x36,0x08,0x00},		//   ( 93)  } - 0x007D Right Curly Bracket
	{0x02,0x01,0x02,0x04,0x02},		//   ( 94)  ~ - 0x007E Tilde
	{0x3E,0x55,0x55,0x41,0x22},		//   ( 95)  C - 0x0080 <Control>
	{0x00,0x00,0x00,0x00,0x00},		//   ( 96)    - 0x00A0 No-Break Space
	{0x00,0x00,0x79,0x00,0x00},		//   ( 97)  ! - 0x00A1 Inverted Exclamation Mark
	{0x18,0x24,0x74,0x2E,0x24},		//   ( 98)  c - 0x00A2 Cent Sign
	{0x48,0x7E,0x49,0x42,0x40},		//   ( 99)  L - 0x00A3 Pound Sign
	{0x5D,0x22,0x22,0x22,0x5D},		//   (100)  o - 0x00A4 Currency Sign
	{0x15,0x16,0x7C,0x16,0x15},		//   (101)  Y - 0x00A5 Yen Sign
	{0x00,0x00,0x77,0x00,0x00},		//   (102)  | - 0x00A6 Broken Bar
	{0x0A,0x55,0x55,0x55,0x28},		//   (103)    - 0x00A7 Section Sign
	{0x00,0x01,0x00,0x01,0x00},		//   (104)  " - 0x00A8 Diaeresis
	{0x00,0x0A,0x0D,0x0A,0x04},		//   (105)    - 0x00AA Feminine Ordinal Indicator
	{0x08,0x14,0x2A,0x14,0x22},		//   (106) << - 0x00AB Left-Pointing Double Angle Quotation Mark
	{0x04,0x04,0x04,0x04,0x1C},		//   (107)    - 0x00AC Not Sign
	{0x00,0x08,0x08,0x08,0x00},		//   (108)  - - 0x00AD Soft Hyphen
	{0x01,0x01,0x01,0x01,0x01},		//   (109)    - 0x00AF Macron
	{0x00,0x02,0x05,0x02,0x00},		//   (110)    - 0x00B0 Degree Sign
	{0x44,0x44,0x5F,0x44,0x44},		//   (111) +- - 0x00B1 Plus-Minus Sign
	{0x00,0x00,0x04,0x02,0x01},		//   (112)  ` - 0x00B4 Acute Accent
	{0x7E,0x20,0x20,0x10,0x3E},		//   (113)  u - 0x00B5 Micro Sign
	{0x06,0x0F,0x7F,0x00,0x7F},		//   (114)    - 0x00B6 Pilcrow Sign
	{0x00,0x18,0x18,0x00,0x00},		//   (115)  . - 0x00B7 Middle Dot
	{0x00,0x40,0x50,0x20,0x00},		//   (116)    - 0x00B8 Cedilla
	{0x00,0x0A,0x0D,0x0A,0x00},		//   (117)    - 0x00BA Masculine Ordinal Indicator
	{0x22,0x14,0x2A,0x14,0x08},		//   (118) >> - 0x00BB Right-Pointing Double Angle Quotation Mark
	{0x17,0x08,0x34,0x2A,0x7D},		//   (119) /4 - 0x00BC Vulgar Fraction One Quarter
	{0x17,0x08,0x04,0x6A,0x59},		//   (120) /2 - 0x00BD Vulgar Fraction One Half
	{0x30,0x48,0x45,0x40,0x20},		//   (121)  ? - 0x00BF Inverted Question Mark
	{0x70,0x29,0x26,0x28,0x70},		//   (122) `A - 0x00C0 Latin Capital Letter A with Grave
	{0x70,0x28,0x26,0x29,0x70},		//   (123) 'A - 0x00C1 Latin Capital Letter A with Acute
	{0x70,0x2A,0x25,0x2A,0x70},		//   (124) ^A - 0x00C2 Latin Capital Letter A with Circumflex
	{0x72,0x29,0x26,0x29,0x70},		//   (125) ~A - 0x00C3 Latin Capital Letter A with Tilde
	{0x70,0x29,0x24,0x29,0x70},		//   (126) "A - 0x00C4 Latin Capital Letter A with Diaeresis
	{0x70,0x2A,0x2D,0x2A,0x70},		//   (127)  A - 0x00C5 Latin Capital Letter A with Ring Above
	{0x7E,0x11,0x7F,0x49,0x49},		//   (128) AE - 0x00C6 Latin Capital Letter Ae
	{0x0E,0x51,0x51,0x71,0x11},		//   (129)  C - 0x00C7 Latin Capital Letter C with Cedilla
	{0x7C,0x55,0x56,0x54,0x44},		//   (130) `E - 0x00C8 Latin Capital Letter E with Grave
	{0x7C,0x55,0x56,0x54,0x44},		//   (131) 'E - 0x00C9 Latin Capital Letter E with Acute
	{0x7C,0x56,0x55,0x56,0x44},		//   (132) ^E - 0x00CA Latin Capital Letter E with Circumflex
	{0x7C,0x55,0x54,0x55,0x44},		//   (133) "E - 0x00CB Latin Capital Letter E with Diaeresis
	{0x00,0x45,0x7E,0x44,0x00},		//   (134) `I - 0x00CC Latin Capital Letter I with Grave
	{0x00,0x44,0x7E,0x45,0x00},		//   (135) 'I - 0x00CD Latin Capital Letter I with Acute
	{0x00,0x46,0x7D,0x46,0x00},		//   (136) ^I - 0x00CE Latin Capital Letter I with Circumflex
	{0x00,0x45,0x7C,0x45,0x00},		//   (137) "I - 0x00CF Latin Capital Letter I with Diaeresis
	{0x7F,0x49,0x49,0x41,0x3E},		//   (138)  D - 0x00D0 Latin Capital Letter Eth
	{0x7C,0x0A,0x11,0x22,0x7D},		//   (139) ~N - 0x00D1 Latin Capital Letter N with Tilde
	{0x38,0x45,0x46,0x44,0x38},		//   (140) `O - 0x00D2 Latin Capital Letter O with Grave
	{0x38,0x44,0x46,0x45,0x38},		//   (141) 'O - 0x00D3 Latin Capital Letter O with Acute
	{0x38,0x46,0x45,0x46,0x38},		//   (142) ^O - 0x00D4 Latin Capital Letter O with Circumflex
	{0x38,0x46,0x45,0x46,0x39},		//   (143) ~O - 0x00D5 Latin Capital Letter O with Tilde
	{0x38,0x45,0x44,0x45,0x38},		//   (144) "O - 0x00D6 Latin Capital Letter O with Diaeresis
	{0x22,0x14,0x08,0x14,0x22},		//   (145)  x - 0x00D7 Multiplcation Sign
	{0x2E,0x51,0x49,0x45,0x3A},		//   (146)  O - 0x00D8 Latin Capital Letter O with Stroke
	{0x3C,0x41,0x42,0x40,0x3C},		//   (147) `U - 0x00D9 Latin Capital Letter U with Grave
	{0x3C,0x40,0x42,0x41,0x3C},		//   (148) 'U - 0x00DA Latin Capital Letter U with Acute
	{0x3C,0x42,0x41,0x42,0x3C},		//   (149) ^U - 0x00DB Latin Capital Letter U with Circumflex
	{0x3C,0x41,0x40,0x41,0x3C},		//   (150) "U - 0x00DC Latin Capital Letter U with Diaeresis
	{0x0C,0x10,0x62,0x11,0x0C},		//   (151) `Y - 0x00DD Latin Capital Letter Y with Acute
	{0x7F,0x12,0x12,0x12,0x0C},		//   (152)  P - 0x00DE Latin Capital Letter Thom
	{0x40,0x3E,0x01,0x49,0x36},		//   (153)  B - 0x00DF Latin Capital Letter Sharp S
	{0x20,0x55,0x56,0x54,0x78},		//   (154) `a - 0x00E0 Latin Small Letter A with Grave
	{0x20,0x54,0x56,0x55,0x78},		//   (155) 'a - 0x00E1 Latin Small Letter A with Acute
	{0x20,0x56,0x55,0x56,0x78},		//   (156) ^a - 0x00E2 Latin Small Letter A with Circumflex
	{0x20,0x55,0x56,0x55,0x78},		//   (157) ~a - 0x00E3 Latin Small Letter A with Tilde
	{0x20,0x55,0x54,0x55,0x78},		//   (158) "a - 0x00E4 Latin Small Letter A with Diaeresis
	{0x20,0x56,0x57,0x56,0x78},		//   (159)  a - 0x00E5 Latin Small Letter A with Ring Above
	{0x24,0x54,0x78,0x54,0x58},		//   (160) ae - 0x00E6 Latin Small Letter Ae
	{0x0C,0x52,0x52,0x72,0x13},		//   (161)  c - 0x00E7 Latin Small Letter c with Cedilla
	{0x38,0x55,0x56,0x54,0x18},		//   (162) `e - 0x00E8 Latin Small Letter E with Grave
	{0x38,0x54,0x56,0x55,0x18},		//   (163) 'e - 0x00E9 Latin Small Letter E with Acute
	{0x38,0x56,0x55,0x56,0x18},		//   (164) ^e - 0x00EA Latin Small Letter E with Circumflex
	{0x38,0x55,0x54,0x55,0x18},		//   (165) "e - 0x00EB Latin Small Letter E with Diaeresis
	{0x00,0x49,0x7A,0x40,0x00},		//   (166) `i - 0x00EC Latin Small Letter I with Grave
	{0x00,0x48,0x7A,0x41,0x00},		//   (167) 'i - 0x00ED Latin Small Letter I with Acute
	{0x00,0x4A,0x79,0x42,0x00},		//   (168) ^i - 0x00EE Latin Small Letter I with Circumflex
	{0x00,0x4A,0x78,0x42,0x00},		//   (169) "i - 0x00EF Latin Small Letter I with Diaeresis
	{0x31,0x4A,0x4E,0x4A,0x30},		//   (170)    - 0x00F0 Latin Small Letter Eth
	{0x7A,0x11,0x0A,0x09,0x70},		//   (171) ~n - 0x00F1 Latin Small Letter N with Tilde
	{0x30,0x49,0x4A,0x48,0x30},		//   (172) `o - 0x00F2 Latin Small Letter O with Grave
	{0x30,0x48,0x4A,0x49,0x30},		//   (173) 'o - 0x00F3 Latin Small Letter O with Acute
	{0x30,0x4A,0x49,0x4A,0x30},		//   (174) ^o - 0x00F4 Latin Small Letter O with Circumflex
	{0x30,0x4A,0x49,0x4A,0x31},		//   (175) ~o - 0x00F5 Latin Small Letter O with Tilde
	{0x30,0x4A,0x48,0x4A,0x30},		//   (176) "o - 0x00F6 Latin Small Letter O with Diaeresis
	{0x08,0x08,0x2A,0x08,0x08},		//   (177)  + - 0x00F7 Division Sign
	{0x38,0x64,0x54,0x4C,0x38},		//   (178)  o - 0x00F8 Latin Small Letter O with Stroke
	{0x38,0x41,0x42,0x20,0x78},		//   (179) `u - 0x00F9 Latin Small Letter U with Grave
	{0x38,0x40,0x42,0x21,0x78},		//   (180) 'u - 0x00FA Latin Small Letter U with Acute
	{0x38,0x42,0x41,0x22,0x78},		//   (181) ^u - 0x00FB Latin Small Letter U with Circumflex
	{0x38,0x42,0x40,0x22,0x78},		//   (182) "u - 0x00FC Latin Small Letter U with Diaeresis
	{0x0C,0x50,0x52,0x51,0x3C},		//   (183) 'y - 0x00FD Latin Small Letter Y with Acute
	{0x7E,0x14,0x14,0x14,0x08},		//   (184)  p - 0x00FE Latin Small Letter Thom
	{0x0C,0x51,0x50,0x51,0x3C},		//   (185) "y - 0x00FF Latin Small Letter Y with Diaeresis
	{0x1E,0x09,0x09,0x29,0x5E},		//   (186)  A - 0x0104 Latin Capital Letter A with Ogonek
	{0x08,0x15,0x15,0x35,0x4E},		//   (187)  a - 0x0105 Latin Small Letter A with Ogonek
	{0x38,0x44,0x46,0x45,0x20},		//   (188) 'C - 0x0106 Latin Capital Letter C with Acute
	{0x30,0x48,0x4A,0x49,0x20},		//   (189) 'c - 0x0107 Latin Small Letter C with Acute
	{0x38,0x45,0x46,0x45,0x20},		//   (190)  C - 0x010C Latin Capital Letter C with Caron
	{0x30,0x49,0x4A,0x49,0x20},		//   (191)  c - 0x010D Latin Small Letter C with Caron
	{0x7C,0x45,0x46,0x45,0x38},		//   (192)  D - 0x010E Latin Capital Letter D with Caron
	{0x20,0x50,0x50,0x7C,0x03},		//   (193) d' - 0x010F Latin Small Letter D with Caron
	{0x1F,0x15,0x15,0x35,0x51},		//   (194)  E - 0x0118 Latin Capital Letter E with Ogonek
	{0x0E,0x15,0x15,0x35,0x46},		//   (195)  e - 0x0119 Latin Small Letter E with Ogonek
	{0x7C,0x55,0x56,0x55,0x44},		//   (196)  E - 0x011A Latin Capital Letter E with Caron
	{0x38,0x55,0x56,0x55,0x18},		//   (197)  e - 0x011B Latin Small Letter E with Caron
	{0x00,0x44,0x7C,0x40,0x00},		//   (198)  i - 0x0131 Latin Small Letter Dotless I
	{0x7F,0x48,0x44,0x40,0x40},		//   (199)  L - 0x0141 Latin Capital Letter L with Stroke
	{0x00,0x49,0x7F,0x44,0x00},		//   (200)  l - 0x0142 Latin Small Letter L with Stroke
	{0x7C,0x08,0x12,0x21,0x7C},		//   (201) 'N - 0x0143 Latin Capital Letter N with Acute
	{0x78,0x10,0x0A,0x09,0x70},		//   (202) 'n - 0x0144 Latin Small Letter N with Acute
	{0x7C,0x09,0x12,0x21,0x7C},		//   (203)  N - 0x0147 Latin Capital Letter N with Caron
	{0x78,0x11,0x0A,0x09,0x70},		//   (204)  n - 0x0148 Latin Small Letter N with Caron
	{0x38,0x47,0x44,0x47,0x38},		//   (205) "O - 0x0150 Latin Capital Letter O with Double Acute
	{0x30,0x4B,0x48,0x4B,0x30},		//   (206) "o - 0x0151 Latin Small Letter O with Double Acute
	{0x3E,0x41,0x7F,0x49,0x49},		//   (207) OE - 0x0152 Latin Capital Ligature Oe
	{0x38,0x44,0x38,0x54,0x58},		//   (208) oe - 0x0153 Latin Small Ligature Oe
	{0x7C,0x15,0x16,0x35,0x48},		//   (209)  R - 0x0158 Latin Capital Letter R with Caron
	{0x78,0x11,0x0A,0x09,0x10},		//   (210)  r - 0x0159 Latin Small Letter R with Caron
	{0x48,0x54,0x56,0x55,0x20},		//   (211) 'S - 0x015A Latin Capital Letter S with Acute
	{0x20,0x48,0x56,0x55,0x20},		//   (212) 's - 0x015B Latin Small Letter S with Acute
	{0x48,0x55,0x56,0x55,0x20},		//   (213)  S - 0x0160 Latin Capital Letter S with Caron
	{0x20,0x49,0x56,0x55,0x20},		//   (214)  s - 0x0161 Latin Small Letter S with Caron
	{0x04,0x05,0x7E,0x05,0x04},		//   (215)  T - 0x0164 Latin Capital Letter T with Caron
	{0x08,0x3C,0x48,0x22,0x01},		//   (216) t' - 0x0165 Latin Small Letter T with Caron
	{0x3C,0x42,0x45,0x42,0x3C},		//   (217)  U - 0x016E Latin Capital Letter U with Ring Above
	{0x38,0x42,0x45,0x22,0x78},		//   (218)  u - 0x016F Latin Small Letter U with Ring Above
	{0x3C,0x43,0x40,0x43,0x3C},		//   (219) "U - 0x0170 Latin Capital Letter U with Double Acute
	{0x38,0x43,0x40,0x23,0x78},		//   (220) "u - 0x0171 Latin Small Letter U with Double Acute
	{0x0C,0x11,0x60,0x11,0x0C},		//   (221) "Y - 0x0178 Latin Capital Letter Y with Diaeresis
	{0x44,0x66,0x55,0x4C,0x44},		//   (222) 'Z - 0x0179 Latin Capital Letter Z with Acute
	{0x48,0x6A,0x59,0x48,0x00},		//   (223) 'z - 0x017A Latin Small Letter Z with Acute
	{0x44,0x64,0x55,0x4C,0x44},		//   (224)  Z - 0x017B Latin Capital Letter Z with Dot Above
	{0x48,0x68,0x5A,0x48,0x00},		//   (225)  z - 0x017C Latin Small Letter Z with Dot Above
	{0x44,0x65,0x56,0x4D,0x44},		//   (226)  Z - 0x017D Latin Capital Letter Z with Caron
	{0x48,0x69,0x5A,0x49,0x00},		//   (227)  z - 0x017E Latin Small Letter Z with Caron
	{0x00,0x02,0x01,0x02,0x00},		//   (228)  ^ - 0x02C6 Modifier Letter Circumflex Accent
	{0x00,0x01,0x02,0x01,0x00},		//   (229)    - 0x02C7 Caron
	{0x00,0x01,0x01,0x01,0x00},		//   (230)    - 0x02C9 Modifier Letter Macron
	{0x01,0x02,0x02,0x01,0x00},		//   (231)    - 0x02D8 Breve
	{0x00,0x00,0x01,0x00,0x00},		//   (232)    - 0x02D9 Dot Above
	{0x00,0x02,0x05,0x02,0x00},		//   (233)    - 0x02DA Ring Above
	{0x02,0x01,0x02,0x01,0x00},		//   (234)  ~ - 0x02DC Small Tilde
	{0x7F,0x05,0x15,0x3A,0x50},		//   (235) Pt - 0x20A7 Peseta Sign
	{0x3E,0x55,0x55,0x41,0x22},		//   (236)  C - 0x20AC Euro Sign
	{0x18,0x14,0x08,0x14,0x0C},		//   (237)    - 0x221E Infinity
	{0x44,0x4A,0x4A,0x51,0x51},		//   (238)  < - 0x2264 Less-Than or Equal to
	{0x51,0x51,0x4A,0x4A,0x44},		//   (239)  > - 0x2265 Greater-Than or Equal to
	{0x74,0x42,0x41,0x42,0x74},		//   (240)    - 0x2302 House
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Character (5x7)
//
//    a: Database
//    b: Ascii
//    c: Start X Address
//    d: Start Y Address
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Font57(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	if(b == ' ')
	{
		b = 0x60;
	}
	else
	{
		b-=0x20;
	}
		
	unsigned char *Src_Pointer;
	unsigned char i,Font,MSB_1,LSB_1,MSB_2,LSB_2;
	//unsigned char derp = d + 16;

	switch(a)
	{
		case 1:
		Src_Pointer=(&(Ascii_2[(b-1)][0]));
		break;
		//case 2:
		//Src_Pointer=&Ascii_2[(b-1)][0];
		//break;
	}

	set_remap_format(0x15);
	for(i=0;i<=1;i++)
	{
		MSB_1= *Src_Pointer;
		Src_Pointer++;
		if(i == 1)
		{
			LSB_1=0x00;
			MSB_2=0x00;
			LSB_2=0x00;
		}
		else
		{
			LSB_1=*Src_Pointer;
			Src_Pointer++;
			MSB_2=*Src_Pointer;
			Src_Pointer++;
			LSB_2=*Src_Pointer;
			Src_Pointer++;
		}
		set_column_address(Shift+c,Shift+c);
		set_row_address(d,d+7);
		set_write_RAM();
		
		Font=((MSB_1&0x01)<<4)|(LSB_1&0x01);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		Font=((MSB_2&0x01)<<4)|(LSB_2&0x01);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		
		Font=((MSB_1&0x02)<<3)|((LSB_1&0x02)>>1);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		Font=((MSB_2&0x02)<<3)|((LSB_2&0x02)>>1);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		
		Font=((MSB_1&0x04)<<2)|((LSB_1&0x04)>>2);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		Font=((MSB_2&0x04)<<2)|((LSB_2&0x04)>>2);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		
		Font=((MSB_1&0x08)<<1)|((LSB_1&0x08)>>3);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		Font=((MSB_2&0x08)<<1)|((LSB_2&0x08)>>3);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_write_data(Font);
		
		Font=((MSB_1&0x10)<<3)|((LSB_1&0x10)>>1);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);
		Font=((MSB_2&0x10)<<3)|((LSB_2&0x10)>>1);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);

		Font=((MSB_1&0x20)<<2)|((LSB_1&0x20)>>2);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);
		Font=((MSB_2&0x20)<<2)|((LSB_2&0x20)>>2);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);

		Font=((MSB_1&0x40)<<1)|((LSB_1&0x40)>>3);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);
		Font=((MSB_2&0x40)<<1)|((LSB_2&0x40)>>3);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);

		Font=(MSB_1&0x80)|((LSB_1&0x80)>>4);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);
		Font=(MSB_2&0x80)|((LSB_2&0x80)>>4);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_write_data(Font);
		
		c++;
	}
	set_remap_format(0x14);
	
}
