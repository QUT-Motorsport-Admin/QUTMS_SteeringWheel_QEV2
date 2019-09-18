/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/main.c
* @author  Zoe Goodward
* @version V1.0.0
* @date    2/08/2019 1:12:41 PM
* @brief   Main entry point of program
*****************************************************************************/

#define F_CPU 16000000UL /* CPU clock in Hertz */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "UART.h"
#include "spi.h"
#include "OLED.h"
#include "images.h"
#include "input.h"
#include "adc.h"

// Vertical scroll
#define UPWARD 0x00
#define DOWNWARD 0x01

// Fade scroll
#define UPWARD_OUT 0x02
#define DOWNWARD_OUT 0x03


/*============================================================================
Function:   data_processing()
------------------------------------------------------------------------------
Purpose :   turns 1 byte of B/W data into 4 byte gray data
Input   :   unsigned char temp -
Returns :   return
Notes   :
============================================================================*/
void data_processing ( uint8_t temp )
{
    uint8_t temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
    uint8_t h11, h12, h13, h14, h15, h16, h17, h18, d1, d2, d3, d4;

    temp1 = temp & 0b10000000;			// 0b10000000
    temp2 = ( temp & 0b01000000 ) >> 3; // 0b00001000
    temp3 = ( temp & 0b00100000 ) << 2; // 0b10000000
    temp4 = ( temp & 0b00010000 ) >> 1; // 0b00001000
    temp5 = ( temp & 0b00001000 ) << 4; // 0b10000000
    temp6 = ( temp & 0b00000100 ) << 1; // 0b00001000
    temp7 = ( temp & 0b00000010 ) << 6; // 0b10000000
    temp8 = ( temp & 0b00000001 ) << 3; // 0b00001000
    h11 = temp1 | temp1 >> 1 | temp1 >> 2 | temp1 >> 3;
    h12 = temp2 | temp2 >> 1 | temp2 >> 2 | temp2 >> 3;
    h13 = temp3 | temp3 >> 1 | temp3 >> 2 | temp3 >> 3;
    h14 = temp4 | temp4 >> 1 | temp4 >> 2 | temp4 >> 3;
    h15 = temp5 | temp5 >> 1 | temp5 >> 2 | temp5 >> 3;
    h16 = temp6 | temp6 >> 1 | temp6 >> 2 | temp6 >> 3;
    h17 = temp7 | temp7 >> 1 | temp7 >> 2 | temp7 >> 3;
    h18 = temp8 | temp8 >> 1 | temp8 >> 2 | temp8 >> 3;
    d1 = h11 | h12;
    d2 = h13 | h14;
    d3 = h15 | h16;
    d4 = h17 | h18;

    OLED_write_data ( d1 );
    OLED_write_data ( d2 );
    OLED_write_data ( d3 );
    OLED_write_data ( d4 );
}


/*============================================================================
Function:   display_picture()
------------------------------------------------------------------------------
Purpose :   displays a bitmap image on the OLED screen
Input   :   unsigned char pic[] - an array of hexidecimal values
Returns :   return
Notes   :
============================================================================*/

void display_picture ( const unsigned char pic[] )
{
    uint8_t i, j;
    OLED_set_column_address ( 0x00, 0x77 );
    OLED_set_row_address ( 0x00, 0x7F );
    OLED_enable_write_RAM();

    for ( i = 0; i < 64; i++ )
    {
        for ( j = 0; j < 60; j++ )
        {
            data_processing ( pgm_read_byte ( &pic[i * 32 + j] ) );
        }
    }
    return;
}

/*============================================================================
Function:   splash_screen()
------------------------------------------------------------------------------
Purpose :   displays the QUT Motorsport logo when the steering wheel system
is first initialised / alternatively can display party parrot
Input   :   none
Returns :   void
Notes   :
============================================================================*/
void splash_screen()
{
    OLED_fill_ram ( 0 );
    display_picture ( MOTORSPORT );
    _delay_ms ( 3000 );
    OLED_fill_ram ( 0 );
    // Add transition effect

}

/*============================================================================
Function:   steering_wheel_init()
------------------------------------------------------------------------------
Purpose :   consolidates all the functions required for the initialisation of
the steering wheel
Input   :   none
Returns :   void
Notes   :
============================================================================*/
void steering_wheel_init()
{
    DDRD |= 0b11001000; // LEDs as outputs, DC
    /* Set MOSI and SCK output, all others input */
    DDRB |= 0b10100011; // CS/SS, reset
    DDRA |= 0b00000000; // Set buttons as input
    // TXD/MOSI_A, OLED_CS
    SPI_CLOCK_HIGH;
    //uart0_init(9600);
    adc_init();
    spi_init();
    OLED_init();
    sei(); // Enable interrupts
    //splash_screen();
}

uint16_t pot = 0;

uint8_t n = 123;
uint8_t buffer[8];

unsigned char TempBuffer[10];

//uint32_t data = 0;

int main ( void )
{
    steering_wheel_init();

    Show_Pixel ( 0, 0, 1 );
    Show_Pixel ( 1, 0, 1 );
    Show_Pixel ( 2, 0, 1 );
    Show_Pixel ( 3, 0, 1 );
    Show_Pixel ( 4, 0, 1 );
    //Show_Pixel ( 0, 10, 1 );
    Present_Buffer();
    //_delay_ms ( 1000 );
    //Show_Pixel ( 0, 10, 0 );
    //Show_Pixel ( 10, 0, 1 );
    //Show_Pixel ( 10, 10, 1 );
    Show_String(50, 20, "1234567890");
    Present_Buffer();

    while ( 1 )
    {

        _delay_ms ( 50 );
        //fill_RAM(CLEAR_SCREEN);
        /* Button pin change testing - validated */
        //if(!(PINA & (1 << BTN_C))) {
        //LED_A_OFF;
        //} else {
        //LED_A_ON;
        //}
        /* Testing and validating ADC implementation */

        pot = adc_read ( 0 );
        /* UART */
        //uart0_transmit(pot >> 2);
        //uart0_transmit(pot);
        //itoa(pot,TempBuffer,10);
        //Show_String(1,TempBuffer,0x28,0x05);


        _delay_ms ( 50 );



        if ( pot >= 0 && pot < 250 ) {
            LED_A_OFF;
            LED_B_OFF;
        } else if ( pot >= 250 && pot < 500 ) {
            LED_A_ON;
            LED_B_OFF;
        } else if ( pot >= 500 && pot < 750 ) {
            LED_A_OFF;
            LED_B_ON;
        } else if ( pot >= 750 ) {
            LED_A_ON;
            LED_B_ON;
        }
        //Present_Buffer();
    }


}