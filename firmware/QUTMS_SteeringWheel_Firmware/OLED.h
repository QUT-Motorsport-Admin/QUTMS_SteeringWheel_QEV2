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

#include <avr/io.h>
#include <stdint.h>

#define OLED_CS_HIGH PORTB |= 0b00000010
#define OLED_CS_LOW PORTB &= ~0b00000010

#define OLED_RESET_HIGH PORTB |= 0b00000001
#define OLED_RESET_LOW PORTB &= ~0b00000001

#define SPI_CLOCK_HIGH PORTB |= 0b10000000
#define SPI_CLOCK_LOW PORTB &= ~0b10000000

#define SPI_DATA_HIGH PORTB |= 0b00100000
#define SPI_DATA_LOW PORTB &= ~0b00100000

// current transmission mode
#define OLED_DC_DATA PORTD |= 0b00001000         // D4 ON
#define OLED_DC_INSTRUCTION PORTD &= ~0b00001000 // D4 OFF

#define OLED_BRIGHTNESS 0x0F
#define OLED_COLUMN_SHIFT 0x1C

// commands
#define OLED_CMD_ENABLE_GRAY_SCALE 0x00
#define OLED_CMD_ADDRESS_COLUMN 0x15
#define OLED_CMD_ADDRESS_ROW 0x75
#define OLED_CMD_RAM_WRITE 0x5C
#define OLED_CMD_RAM_READ 0x5D
#define OLED_CMD_REMAP_AND_DUAL_COM_LINE 0xA0
#define OLED_CMD_DISPLAY_START_LINE 0xA1
#define OLED_CMD_DISPLAY_OFFSET 0xA2
#define OLED_CMD_DISPLAYMODE 0xA4
#define OLED_CMD_PARTIAL_DISPLAY_ENABLE 0xA8
#define OLED_CMD_PARTIAL_DISPLAY_EXIT 0xA9
#define OLED_CMD_FUNCTION_SELECTION 0xAB
#define OLED_CMD_DISPLAY_ON 0xAF
#define OLED_CMD_DISPLAY_OFF 0xAE
#define OLED_CMD_PHASE_LENGTH 0xB1
#define OLED_CMD_FRONT_CLOCK 0xB3
#define OLED_CMD_DISPLAY_ENHANCEMENT_A 0xB4
#define OLED_CMD_GPIO 0xB5
#define OLED_CMD_PRECHARGE_PERIOD 0xB6
#define OLED_CMD_GRAY_SCALE_TABLE 0xB8
#define OLED_CMD_GRAY_SCALE_DEFAULT 0xB9
#define OLED_CMD_PRECHARGE_VOLTAGE 0xBB
#define OLED_CMD_VOLTAGE_COM 0xBE
#define OLED_CMD_CONTRAST_CURRENT 0xC1
#define OLED_CMD_MASTER_CONTRAST 0xC7
#define OLED_CMD_MUX_RATIO 0xCA
#define OLED_CMD_DISPLAY_ENHANCEMENT_B 0xD1
#define OLED_CMD_COMMAND_LOCK 0xFD

typedef enum OLED_DISPLAY_MODES {
    OLED_DISPLAYMODE_OFF = 0xA4,
    OLED_DISPLAYMODE_ON = 0xA5,
    OLED_DISPLAYMODE_NORMAL = 0xA6,
    OLED_DISPLAYMODE_INVERSE = 0xA7,
} OLED_DISPLAY_MODES;

typedef enum OLED_FUNCTION {
    OLED_FUNCTION_EXTERNALVDD = 0,
    OLED_FUNCTION_INTERNALVDD = 1
} OLED_FUNCTION;

typedef enum OLED_ENHANCEMENT_VSL {
    OLED_ENHANCEMENT_VSL_EXTERNAL = 0b00,
    OLED_ENHANCEMENT_VSL_INTERNAL = 0b10
} OLED_ENHANCEMENT_VSL;

typedef enum OLED_ENHANCEMENT_GS_QUALITY {
    OLED_ENHANCEMENT_GS_QUALITY_LOW = 0b11111,
    OLED_ENHANCEMENT_GS_QUALITY_NORMAL = 0b10110
} OLED_ENHANCEMENT_GS_QUALITY;

typedef enum OLED_GPIO_SETTINGS {
    OLED_GPIO_INPUT_DISABLED = 0b00,
    OLED_GPIO_INPUT_ENABLED = 0b01,
    OLED_GPIO_OUTPUT_LOW = 0b10,
    OLED_GPIO_OUTPUT_HIGH = 0b11,
} OLED_GPIO_SETTINGS;

typedef enum OLED_CLOCK_DIVIDE_RATIO {
    OLED_CLOCK_DIVIDE_RATIO_1 = 0b0000,
    OLED_CLOCK_DIVIDE_RATIO_2 = 0b0001,
    OLED_CLOCK_DIVIDE_RATIO_4 = 0b0010,
    OLED_CLOCK_DIVIDE_RATIO_8 = 0b0011,
    OLED_CLOCK_DIVIDE_RATIO_16 = 0b0100,
    OLED_CLOCK_DIVIDE_RATIO_32 = 0b0101,
    OLED_CLOCK_DIVIDE_RATIO_64 = 0b0110,
    OLED_CLOCK_DIVIDE_RATIO_128 = 0b0111,
    OLED_CLOCK_DIVIDE_RATIO_256 = 0b1000,
    OLED_CLOCK_DIVIDE_RATIO_512 = 0b1001,
    OLED_CLOCK_DIVIDE_RATIO_1024 = 0b1010,
} OLED_CLOCK_DIVIDE_RATIO;

typedef enum OLED_ENHANCEMENT_B {
    OLED_ENHANCEMENT_B_RESERVED = 0b00,
    OLED_ENHANCEMENT_B_NORMAL = 0b10
} OLED_ENHANCEMENT_B;

typedef enum OLED_COMMAND_LOCK {
    OLED_COMMAND_LOCK_UNLOCK = 0,
    OLED_COMMAND_LOCK_LOCKED = 1
} OLED_COMMAND_LOCK;

// Instruction Setting Methods
// for now refer to docs
void OLED_write_data(uint8_t data);
void OLED_write_instruction(uint8_t instruction);

void OLED_enable_gray_scale_table();
void OLED_set_column_address(uint8_t startAddress, uint8_t endAddress);
void OLED_set_row_address(uint8_t startAddress, uint8_t endAddress);
void OLED_enable_write_RAM();
void OLED_enable_read_RAM();
void OLED_set_remap_and_dual_com_line(uint8_t verticalAddressIncrement, uint8_t enableColumnAddressRemap,
                                      uint8_t enableNibbleReamp, uint8_t scanFromCOMMultiplex,
                                      uint8_t enableCOMSplitOddEven, uint8_t enableDualCOMMode);
void OLED_set_display_start_line(uint8_t startLine);
void OLED_set_display_offset(uint8_t verticalScrollOffset);
void OLED_set_display_mode(OLED_DISPLAY_MODES displayMode);
void OLED_set_partial_display(uint8_t enablePartial, uint8_t startRow, uint8_t endRow);
void OLED_set_function_selection(OLED_FUNCTION function);
void OLED_set_display_off();
void OLED_set_display_on();
void OLED_set_phase_length(uint8_t phase1Period, uint8_t phase2Period);
void OLED_set_front_clock(OLED_CLOCK_DIVIDE_RATIO divideRatio, uint8_t frequency);
void OLED_set_display_enhancement_A(OLED_ENHANCEMENT_VSL vsl, OLED_ENHANCEMENT_GS_QUALITY gsQuality);
void OLED_set_GPIO(OLED_GPIO_SETTINGS gpio1Settings, OLED_GPIO_SETTINGS gpio2Settings);
void OLED_set_second_precharge_period(uint8_t period);
void OLED_set_gray_scale_table_oled_default();
void OLED_set_gray_scale_table(uint8_t gs1, uint8_t gs2, uint8_t gs3, uint8_t gs4, uint8_t gs5,
                               uint8_t gs6, uint8_t gs7, uint8_t gs8, uint8_t gs9, uint8_t gs10,
                               uint8_t gs11, uint8_t gs12, uint8_t gs13, uint8_t gs14, uint8_t gs15);
void OLED_set_gray_scale_table_default();
void OLED_set_precharge_voltage(uint8_t voltage);
void OLED_set_deselect_voltage(uint8_t voltage);
void OLED_set_contrast_current(uint8_t contrast);
void OLED_set_master_contrast_current_control(uint8_t control);
void OLED_set_MUX_ratio(uint8_t ratio);
void OLED_set_display_enhancement_B(OLED_ENHANCEMENT_B enhancement);
void OLED_set_command_lock(OLED_COMMAND_LOCK lock);

void OLED_init();
void OLED_fill_ram(uint8_t data);

// wrapper functions

#define OLED_Y 64
#define OLED_X 256

#define OLED_ROWS OLED_Y
#define OLED_COLUMNS OLED_X / 4

#define CHAR_HEIGHT 8
#define CHAR_WIDTH 5

#define BIG_CHAR_MOD 3
#define CHAR_HEIGHT_BIG (CHAR_HEIGHT * BIG_CHAR_MOD)
#define CHAR_WIDTH_BIG (CHAR_WIDTH * BIG_CHAR_MOD)

uint8_t oledBuffer[OLED_ROWS * OLED_COLUMNS];
uint8_t oledBufferUpdated[OLED_ROWS * OLED_COLUMNS / 8];

/**
 * Presents the content of the current buffer to the OLED screen
 */
void Present_Buffer();

/**
 * Empties the entire buffer, and clears the screen
 */
void Clear_Buffer();

/**
 * Sets the pixel at (x,y) in the buffer to value
 *
 *	x: the horizontal coordinate
 *	y: the vertical coordinate
 *	value: the bit to display (1 or 0)
 *
 * @return void
 */
void Show_Pixel(uint16_t x, uint16_t y, uint8_t value);

// i hope these are self explanatory if not see CAB202
void Show_Char(uint16_t x, uint16_t y, char value);
void Show_Char_Big(uint16_t x, uint16_t y, char value);
void Show_String(uint16_t x, uint16_t y, char *value);
void Show_String_Big(uint16_t x, uint16_t y, char *value);
void Show_Formatted(uint16_t x, uint16_t y, char *format, ...);

void Show_Progress_Bar(uint16_t x, uint16_t y, uint8_t width, uint8_t height, double percentage);

void display_picture(const unsigned char pic[]);

#endif /* OLED_H_ */