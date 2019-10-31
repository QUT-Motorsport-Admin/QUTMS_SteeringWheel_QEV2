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
#include "menu.h"
#include "settings.h"

// Vertical scroll
#define UPWARD 0x00
#define DOWNWARD 0x01

// Fade scroll
#define UPWARD_OUT 0x02
#define DOWNWARD_OUT 0x03



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
    Clear_Buffer();
    display_picture ( MOTORSPORT );
    Present_Buffer();
    _delay_ms ( 3000 );
    Clear_Buffer();
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
    splash_screen();
}

uint16_t pot = 0;
uint16_t old_pot = 0;

uint8_t n = 123;
uint8_t buffer[8];

unsigned char TempBuffer[10];

//uint32_t data = 0;

int main ( void )
{
    steering_wheel_init();

    /*Show_Pixel ( 0, 0, 1 );
    Show_Pixel ( 1, 0, 1 );
    Show_Pixel ( 2, 0, 1 );
    Show_Pixel ( 3, 0, 1 );
    Show_Pixel ( 4, 0, 1 );*/
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
        //Clear_Buffer();
        _delay_ms ( 50 );
        //fill_RAM(CLEAR_SCREEN);
        /* Button pin change testing - validated */
        //if(!(PINA & (1 << BTN_C))) {
        //LED_A_OFF;
        //} else {
        //LED_A_ON;
        //}
        /* Testing and validating ADC implementation */

        //Show_Pixel((double)old_pot / OLED_COLUMNS, 30, 0);
        pot = adc_read ( 0 );
        //Show_Pixel((double)pot / OLED_COLUMNS, 30, 1);
        old_pot = pot;
        /* UART */
        //uart0_transmit(pot >> 2);
        //uart0_transmit(pot);
        //itoa(pot,TempBuffer,10);
        //Show_String(1,TempBuffer,0x28,0x05);
        Configuration config;
        menu_handle_screens(&config);

        //Show_Formatted(20, 20, "%04d", pot);
        Present_Buffer();


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