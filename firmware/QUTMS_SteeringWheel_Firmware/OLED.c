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
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>
#include "OLED.h"
#include "spi.h"
#include "font.h"

/*============================================================================
Function:   oled_init()
------------------------------------------------------------------------------
Purpose :   initialises the OLED screen
Input   :   none
Returns :   void
Notes   :
============================================================================*/
void OLED_init ( void )
{
    OLED_RESET_LOW;
    _delay_ms ( 1000 );
    OLED_RESET_HIGH;

    // Unlock Basic Commands (0x12/0x16)
    OLED_set_command_lock ( OLED_COMMAND_LOCK_UNLOCK );

    // Display Off (0x00/0x01)
    OLED_set_display_off();

    // Set Clock as 80 Frames/Sec
    OLED_set_front_clock ( OLED_CLOCK_DIVIDE_RATIO_2, 0b1001 );

    // 1/64 Duty (0x0F~0x3F)
    OLED_set_MUX_ratio ( 0x3F );

    // Shift Mapping RAM Counter (0x00~0x3F)
    OLED_set_display_offset ( 0 );

    // Set Mapping RAM Display Start Line (0x00~0x7F)
    OLED_set_display_start_line ( 0 );

    //	Set Horizontal Address Increment
    //	Column Address 0 Mapped to SEG0
    //	Disable Nibble Remap
    //	Scan from COM[N-1] to COM0
    //	Disable COM Split Odd Even
    //	Enable Dual COM Line Mode
    OLED_set_remap_and_dual_com_line ( 0, 0, 1, 1, 0, 1 );

    // Disable GPIO Pins Input
    OLED_set_GPIO ( OLED_GPIO_INPUT_DISABLED, OLED_GPIO_INPUT_DISABLED );

    // Enable Internal VDD Regulator
    OLED_set_function_selection ( OLED_FUNCTION_INTERNALVDD );

    // Enable External VSL
    OLED_set_display_enhancement_A ( OLED_ENHANCEMENT_VSL_EXTERNAL, OLED_ENHANCEMENT_GS_QUALITY_LOW );

    // Set Segment Output Current
    OLED_set_contrast_current ( 0x7F );

    // Set Scale Factor of Segment Output Current Control
    OLED_set_master_contrast_current_control ( OLED_BRIGHTNESS );

    // Set Pulse Width for Gray Scale Table
    OLED_set_gray_scale_table_oled_default();

    // Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
    OLED_set_phase_length ( 5, 14 );

    // Enhance Driving Scheme Capability (0x00/0x20)
    OLED_set_display_enhancement_B ( OLED_ENHANCEMENT_B_RESERVED );

    // Set Pre-Charge Voltage Level as 0.60*VCC
    OLED_set_precharge_voltage ( 0x1F );

    // Set Second Pre-Charge Period as 8 Clocks
    OLED_set_second_precharge_period ( 8 );

    // Set Common Pins Deselect Voltage Level as 0.86*VCC
    OLED_set_deselect_voltage ( 0x07 );

    // Normal Display Mode (0x00/0x01/0x02/0x03)
    OLED_set_display_mode ( OLED_DISPLAYMODE_NORMAL );

    // Disable Partial Display
    OLED_set_partial_display ( 0, 0, 0 );

    // Clear Screen
    OLED_fill_ram ( 0 );

    // Display On (0x00/0x01)
    OLED_set_display_on();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_write_data ( uint8_t data )
{
    OLED_CS_LOW; // pull low to prepare to receive
    OLED_DC_DATA; // we're sending data
    spi_send_byte ( data );
    OLED_DC_DATA;
    OLED_CS_HIGH; // finished transmitting
}

void OLED_write_instruction ( uint8_t data )
{
    OLED_CS_LOW; // pull low to prepare to receive
    OLED_DC_INSTRUCTION; // we're sending commands
    spi_send_byte ( data );
    OLED_DC_DATA;
    OLED_CS_HIGH; // finished transmitting
}

void OLED_enable_gray_scale_table() {
    OLED_write_instruction ( OLED_CMD_ENABLE_GRAY_SCALE );
}

void OLED_set_column_address ( uint8_t startAddress, uint8_t endAddress ) {
    OLED_write_instruction ( OLED_CMD_ADDRESS_COLUMN );
    OLED_write_data ( startAddress + OLED_COLUMN_SHIFT );
    OLED_write_data ( endAddress + OLED_COLUMN_SHIFT );
}

void OLED_set_row_address ( uint8_t startAddress, uint8_t endAddress ) {
    OLED_write_instruction ( OLED_CMD_ADDRESS_ROW );
    OLED_write_data ( startAddress );
    OLED_write_data ( endAddress );
}

void OLED_enable_write_RAM() {
    OLED_write_instruction ( OLED_CMD_RAM_WRITE );
}

void OLED_enable_read_RAM() {
    OLED_write_instruction ( OLED_CMD_RAM_READ );
}

void OLED_set_remap_and_dual_com_line ( uint8_t verticalAddressIncrement, uint8_t enableColumnAddressRemap,
                                        uint8_t enableNibbleReamp, uint8_t scanFromCOMMultiplex,
                                        uint8_t enableCOMSplitOddEven, uint8_t enableDualCOMMode ) {
    uint8_t remapFlags = ( verticalAddressIncrement ) | ( enableColumnAddressRemap << 1 ) | ( enableNibbleReamp << 2 ) | ( scanFromCOMMultiplex << 4 ) | ( enableCOMSplitOddEven << 5 );
    uint8_t comMode = 0b00000001 | ( enableDualCOMMode << 4 );

    OLED_write_instruction ( OLED_CMD_REMAP_AND_DUAL_COM_LINE );
    OLED_write_data ( remapFlags );
    OLED_write_data ( comMode );

}

void OLED_set_display_start_line ( uint8_t startLine ) {
    // screen height is 64, so startLine can be between 0-63, so lop off last 2 bits
    //startLine &= ~( (1 << 6) | (1 << 7));
    OLED_write_instruction ( OLED_CMD_DISPLAY_START_LINE );
    OLED_write_data ( startLine );
}

void OLED_set_display_offset ( uint8_t verticalScrollOffset ) {
    // screen height is 64, so verticalScrollOffset can be between 0-63, so lop off last 2 bits
    //verticalScrollOffset &= ~( (1 << 6) | (1 << 7));
    OLED_write_instruction ( OLED_CMD_DISPLAY_START_LINE );
    OLED_write_data ( verticalScrollOffset );
}

void OLED_set_display_mode ( OLED_DISPLAY_MODES displayMode ) {
    OLED_write_instruction ( displayMode );
    //OLED_write_data((uint8_t)displayMode);
}

void OLED_set_partial_display ( uint8_t enablePartial, uint8_t startRow, uint8_t endRow ) {
    if ( enablePartial == 0 ) {
        OLED_write_instruction ( OLED_CMD_PARTIAL_DISPLAY_EXIT );
    } else {
        // screen height is 64, so make sure startRow and endRow are not
        // bigger than 64
        //startRow &= ~( (1 << 6) | (1 << 7));
        //endRow &= ~( (1 << 6) | (1 << 7));
        if ( startRow > endRow ) {
            return;
        }

        OLED_write_instruction ( OLED_CMD_PARTIAL_DISPLAY_ENABLE );
        OLED_write_data ( startRow );
        OLED_write_data ( endRow );
    }
}

void OLED_set_function_selection ( OLED_FUNCTION function ) {
    uint8_t flags = function;

    OLED_write_instruction ( OLED_CMD_FUNCTION_SELECTION );
    OLED_write_data ( flags );
}

void OLED_set_display_off() {
    OLED_write_instruction ( OLED_CMD_DISPLAY_OFF );
}

void OLED_set_display_on() {
    OLED_write_instruction ( OLED_CMD_DISPLAY_ON );
}

void OLED_set_phase_length ( uint8_t phase1Period, uint8_t phase2Period ) {
    if ( phase1Period < 5 ) {
        phase1Period = 5;
    } else if ( phase1Period > 31 ) {
        phase1Period = 31;
    }

    phase1Period = phase1Period / 2;

    if ( phase2Period < 3 ) {
        phase2Period = 3;
    } else if ( phase2Period > 15 ) {
        phase2Period = 15;
    }

    phase2Period = phase2Period / 2;

    uint8_t flags = ( phase1Period << 0 ) | ( phase2Period << 4 );

    OLED_write_instruction ( OLED_CMD_PHASE_LENGTH );
    OLED_write_data ( flags );
}

void OLED_set_front_clock ( OLED_CLOCK_DIVIDE_RATIO divideRatio, uint8_t frequency ) {
    uint8_t flags = ( divideRatio << 0 ) | ( frequency << 4 );

    OLED_write_instruction ( OLED_CMD_FRONT_CLOCK );
    OLED_write_data ( flags );
}

void OLED_set_display_enhancement_A ( OLED_ENHANCEMENT_VSL vsl, OLED_ENHANCEMENT_GS_QUALITY gsQuality ) {
    uint8_t vslFlag = 0b10100000 | vsl;
    uint8_t gsFlag = 0b00000101 | ( gsQuality << 3 );

    OLED_write_instruction ( OLED_CMD_DISPLAY_ENHANCEMENT_A );
    OLED_write_data ( vslFlag );
    OLED_write_data ( gsFlag );
}

void OLED_set_GPIO ( OLED_GPIO_SETTINGS gpio1Settings, OLED_GPIO_SETTINGS gpio2Settings ) {
    uint8_t flags = ( gpio1Settings << 0 ) | ( gpio2Settings << 2 );

    OLED_write_instruction ( OLED_CMD_GPIO );
    OLED_write_data ( flags );
}

void OLED_set_second_precharge_period ( uint8_t period ) {
    if ( period < 0 ) {
        period = 0;
    } else if ( period > 15 ) {
        period = 15;
    }

    OLED_write_instruction ( OLED_CMD_PRECHARGE_PERIOD );
    OLED_write_data ( period );
}

void OLED_set_gray_scale_table_oled_default() {
    OLED_set_gray_scale_table ( 0x0C, 0x18, 0x24, 0x30,
                                0x3C, 0x48, 0x54, 0x60,
                                0x6C, 0x78, 0x84, 0x90,
                                0x9C, 0xA8, 0xB4 );
}

void OLED_set_gray_scale_table ( uint8_t gs1, uint8_t gs2, uint8_t gs3, uint8_t gs4, uint8_t gs5,
                                 uint8_t gs6, uint8_t gs7, uint8_t gs8, uint8_t gs9, uint8_t gs10,
                                 uint8_t gs11, uint8_t gs12, uint8_t gs13, uint8_t gs14, uint8_t gs15 ) {
    OLED_write_instruction ( OLED_CMD_GRAY_SCALE_TABLE );
    OLED_write_data ( gs1 );			//   Gray Scale Level 1
    OLED_write_data ( gs2 );			//   Gray Scale Level 2
    OLED_write_data ( gs3 );			//   Gray Scale Level 3
    OLED_write_data ( gs4 );			//   Gray Scale Level 4
    OLED_write_data ( gs5 );			//   Gray Scale Level 5
    OLED_write_data ( gs6 );			//   Gray Scale Level 6
    OLED_write_data ( gs7 );			//   Gray Scale Level 7
    OLED_write_data ( gs8 );			//   Gray Scale Level 8
    OLED_write_data ( gs9 );			//   Gray Scale Level 9
    OLED_write_data ( gs10 );			//   Gray Scale Level 10
    OLED_write_data ( gs11 );			//   Gray Scale Level 11
    OLED_write_data ( gs12 );			//   Gray Scale Level 12
    OLED_write_data ( gs13 );			//   Gray Scale Level 13
    OLED_write_data ( gs14 );			//   Gray Scale Level 14
    OLED_write_data ( gs15 );			//   Gray Scale Level 15

    OLED_enable_gray_scale_table();
}

void OLED_set_gray_scale_table_default() {
    OLED_write_instruction ( OLED_CMD_GRAY_SCALE_DEFAULT );
}

void OLED_set_precharge_voltage ( uint8_t voltage ) {
    OLED_write_instruction ( OLED_CMD_PRECHARGE_VOLTAGE );
    OLED_write_data ( voltage );
}

void OLED_set_deselect_voltage ( uint8_t voltage ) {
    OLED_write_instruction ( OLED_CMD_VOLTAGE_COM );
    OLED_write_data ( voltage );
}

void OLED_set_contrast_current ( uint8_t contrast ) {
    OLED_write_instruction ( OLED_CMD_CONTRAST_CURRENT );
    OLED_write_data ( contrast );
}

void OLED_set_master_contrast_current_control ( uint8_t control ) {
    OLED_write_instruction ( OLED_CMD_MASTER_CONTRAST );
    OLED_write_data ( control );
}

void OLED_set_MUX_ratio ( uint8_t ratio ) {
    OLED_write_instruction ( OLED_CMD_MUX_RATIO );
    OLED_write_data ( ratio );
}

void OLED_set_display_enhancement_B ( OLED_ENHANCEMENT_B enhancement ) {
    uint8_t flags = 0b10000010 | ( enhancement << 4 );
    OLED_write_instruction ( OLED_CMD_DISPLAY_ENHANCEMENT_B );
    OLED_write_data ( flags );
    OLED_write_data ( 0x20 ); // ???? from the tech specs lol
}

void OLED_set_command_lock ( uint8_t lock ) {
    uint8_t flags = 0b00010010 | ( lock << 2 );

    OLED_write_instruction ( OLED_CMD_COMMAND_LOCK );
    OLED_write_data ( flags );
}

void OLED_fill_ram ( uint8_t data ) {
    OLED_set_column_address ( 0, OLED_COLUMNS );
    OLED_set_row_address ( 0, OLED_ROWS );
    OLED_enable_write_RAM();

    for ( uint8_t i = 0; i < OLED_ROWS; i++ ) {
        for ( uint8_t j = 0; j < OLED_COLUMNS; j++ ) {
            OLED_write_data ( data );
            OLED_write_data ( data );
        }
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Wrapper Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Present_Buffer() {
    for ( uint8_t i = 0; i < OLED_ROWS; i++ ) {
        for ( uint8_t j = 0; j < OLED_COLUMNS; j++ ) {
            int index = i * OLED_ROWS + j;

            int flagIndex = index / 8;
            int flagPos = index % 8;

            // has this part of the screen changed?
            if ( ( (oledBufferUpdated[flagIndex] >> flagPos) & 1 ) == 1 ) {
                // set bit flag back to zero since we are showing it
                oledBufferUpdated[flagIndex] &= ~ ( 1 << flagPos );

                // currentData is in format 0b d3 d2 d1 d0
                //							0b 00 00 00 00
                uint8_t currentData = oledBuffer[index];
                // extract the 2 bits
                uint8_t d0 = ( currentData & 0b00000011 ) >> 0;
                uint8_t d1 = ( currentData & 0b00001100 ) >> 2;
                uint8_t d2 = ( currentData & 0b00110000 ) >> 4;
                uint8_t d3 = ( currentData & 0b11000000 ) >> 6;

                // now dn is 4 bits
                d0 |= d0 << 2;
                d1 |= d1 << 2;
                d2 |= d2 << 2;
                d3 |= d3 << 2;

                // data0 is now d0d0d1d1
                // data1 is now d2d2d3d3
                uint8_t data0 = ( d0 << 4 ) | d1;
                uint8_t data1 = ( d2 << 4 ) | d3;

                OLED_set_column_address ( j, 0x77 );
                OLED_set_row_address ( i, 0x7F );

                OLED_enable_write_RAM();
                OLED_write_data ( data0 );
                OLED_write_data ( data1 );

            }
        }
    }
}

void Clear_Buffer() {
    for ( uint8_t i = 0; i < OLED_ROWS; i++ ) {
        for ( uint8_t j = 0; j < OLED_COLUMNS; j++ ) {
            int index = i * OLED_ROWS + j;
            oledBuffer[index] = 0;
        }
    }

    OLED_fill_ram(0);
}

void Show_Pixel ( uint16_t x, uint16_t y, uint8_t value ) {
    int col = x / 4;
    int colPos = x % 4;
    int row = y;
    int bufferIndex = row * OLED_ROWS + col;
    int flagIndex = bufferIndex / 8;
    int flagPos = bufferIndex % 8;

    // data should be 0 or 1
    // eg format 0b0000000v
    uint8_t data = ( value == 0 ) ? 0 : 1;

    // make data in the format 0b000000vv
    data |= data << 1;
    uint8_t mask = 0b00000011;

    // shift data across so it's in the correct position for this pixel
    // eg 0b00vv0000
    data = data << (colPos * 2);
    mask = ~(mask << (colPos * 2));

    uint8_t currentData = oledBuffer[bufferIndex] & (mask << (colPos * 2));

    if (currentData != data) {

        // update the buffer with this pixel data
        // clear existing data in this position
        oledBuffer[bufferIndex] &= mask;
        // add the new data
        oledBuffer[bufferIndex] |= data;

        // flag this section as being updated
        oledBufferUpdated[flagIndex] |= ( 1 << flagPos );
    }
}

void Show_Font57 ( uint16_t x, uint16_t y, char value ) {
    if ( value == ' ' ) {
        // set character to '96' (break space)
        value = 0x60;
    } else {
        // offset character by 32 to line it up with our character map
        value -= 0x20;
    }

    // pointer to first element of binary ascii representation
    unsigned char* asciiSrcPointer = Ascii_2[ ( value - 1 )];
    //0001 0101
    //OLED_set_remap_and_dual_com_line(1, 0, 1, 1, 0, 1);

    // loop through each bit
    for ( uint8_t i = 0; i < 8; i++ ) {
        // loop through ascii binary data
        for ( uint8_t j = 0; j < 5; j++ ) {
            uint8_t bit = (( asciiSrcPointer[j] >> i ) & 1) == 0 ? 0 : 1;
            Show_Pixel ( x + j, y + i, bit );
        }
    }
}

void Show_Char ( uint16_t x, uint16_t y, char value ) {
    Show_Font57 ( x, y, value );
}
void Show_String ( uint16_t x, uint16_t y, char* value ) {
    const uint16_t characterWidth = 6;

    int index = 0;

    // loop through value until we get a NULL (c has NULL terminated strings)
    while ( value[index] != 0 ) {
        Show_Char ( x, y, value[index] );
        x += characterWidth;
        index++;
    }
}

void Show_Formatted(uint16_t x, uint16_t y, char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[100];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Show_String(x, y, buffer);
}

void display_picture(const unsigned char pic[]) {
    uint8_t i, j, pix;

    for (i = 0; i < 64; i++) {     // row
        for (j = 0; j < 32; j++) { // column
            for (pix = 0; pix < 8; pix++) {
                uint8_t bytePosition = 7 - pix;
                uint8_t pixel = (pgm_read_byte ( &pic[i * 32 + j] ) & (1 << bytePosition)) >> bytePosition;
                Show_Pixel((8 * j) + pix, i, pixel);
            }

            //data_processing(j, i, pic[i * 32 + j]);
        }
    }
    return;
}