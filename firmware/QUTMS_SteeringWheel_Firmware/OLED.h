/*****************************************************************************
* @file    Software/inc/steeringWheelGraphics.h
* @author  Zoe Goodward
* @version V1.0.0
* @date    3/08/2019 12:47:23 PM
* @brief   This file declares the variables and functions that are used to
*          produce graphics on the OLED screen
*****************************************************************************/

#ifndef OLED_H_
#define OLED_H_

#define OLED_CS_HIGH PORTB |= 0b00000010
#define OLED_CS_LOW PORTB &= ~ 0b00000010
#define OLED_RESET_HIGH	PORTB |= 0b00000001
#define OLED_RESET_LOW PORTB &= ~0b00000001
#define OLED_DC_DATA PORTD |= 0b00001000
#define OLED_DC_INSTRUCTION PORTD &= ~0b00001000
#define SPI_CLOCK_HIGH PORTB |= 0b10000000
#define SPI_CLOCK_LOW PORTB &= ~0b10000000
#define SPI_DATA_HIGH PORTB |= 0b00100000
#define SPI_DATA_LOW PORTB &= ~0b00100000

#define	Shift		0x1C
#define Max_Column	0x3F			// 256/4-1
#define Max_Row		0x3F			// 64-1
#define	BRIGHTNESS	0x0F

#define CLEAR_SCREEN 0x00
#define FILL_SCREEN	 0xFF

void oled_write_data(uint8_t data);
void oled_write_instruction(uint8_t data);
void OLED_init(void);

void set_column_address(uint8_t a, uint8_t b);


void set_row_address(uint8_t a, uint8_t b);


void set_write_RAM();


void set_read_RAM();


void set_remap_format(uint8_t d);

void set_start_line(uint8_t d);


void set_display_offset(uint8_t d);

void set_display_mode(uint8_t d);

void set_partial_display(uint8_t a, uint8_t b, uint8_t c);


void set_function_selection(uint8_t d);


void set_display_on_off(uint8_t d);


void set_phase_length(uint8_t d);


void set_display_clock(uint8_t d);


void set_display_enhancement_A(uint8_t a, uint8_t b);


void set_GPIO(uint8_t d);


void set_precharge_period(uint8_t d);

void set_precharge_voltage(uint8_t d);


void set_VCOMH(uint8_t d);


void set_contrast_current(uint8_t d);


void set_master_current(uint8_t d);


void set_multiplex_ratio(uint8_t d);


void set_display_enhancement_B(uint8_t d);


void set_command_lock(uint8_t d);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void fill_RAM(uint8_t Data);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void set_gray_scale_table();



void Show_Font57(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Show_String(unsigned char a, unsigned char *Data_Pointer, unsigned char b, unsigned char c);


unsigned char Ascii_2[240][5];

#endif /* OLED_H_ */