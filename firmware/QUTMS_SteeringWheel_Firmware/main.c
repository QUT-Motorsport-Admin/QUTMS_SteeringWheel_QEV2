/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/main.c
* @author  Zoe Goodward
* @version V1.0.0
* @date    2/08/2019 1:12:41 PM
* @brief   Main entry point of program
*****************************************************************************/

#define F_CPU 16000000UL /* CPU clock in Hertz */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <util/delay.h>

#include "OLED.h"
#include "UART.h"
#include "adc.h"
#include "drive_menu.h"
#include "images.h"
#include "input.h"
#include "settings.h"
#include "spi.h"
#include "static_menu.h"

#include <stdbool.h>

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
void splash_screen() {
    Clear_Buffer();
    display_picture(MOTORSPORT);
    Present_Buffer();
    _delay_ms(3000);
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
void steering_wheel_init() {
    //DDRD = 0b00000000;
    DDRD |= 0b11001000; // LEDs as outputs, DC
    /* Set MOSI and SCK output, all others input */
    DDRB |= 0b10100011; // CS/SS, reset

    configure_input();
    // TXD/MOSI_A, OLED_CS
    SPI_CLOCK_HIGH;
    //uart0_init(9600);
    adc_init();
    spi_init();

    // initialize screen
    OLED_init();
    // use inverse mode for better visibility
    OLED_set_display_mode(OLED_DISPLAYMODE_INVERSE);

    // setup driving menu inc CANbus
    DM_init();

    sei(); // Enable interrupts
    //splash_screen();
}

uint16_t pot = 0;
uint16_t old_pot = 0;

uint8_t n = 123;
uint8_t buffer[8];

unsigned char TempBuffer[10];

//uint32_t data = 0;

int main(void) {
    //_delay_ms(1000);
    steering_wheel_init();
    OLED_set_display_mode(OLED_DISPLAYMODE_INVERSE);

    Configuration config;
    Configuration *config_address = &config;

    Show_String(0, 0, "test123456789");
    Show_String_Big(0, CHAR_HEIGHT_BIG, "test123456789");
    Present_Buffer();

    while (1) {

        //pot = adc_read(0);
        //Show_Pixel((double)pot / OLED_COLUMNS, 30, 1);
        //old_pot = pot;
        /* UART */
        //uart0_transmit(pot >> 2);
        //uart0_transmit(pot);
        //itoa(pot,TempBuffer,10);
        //Show_String(1,TempBuffer,0x28,0x05);
        //menu_handle_screens(&config_address);

        //Show_Formatted(20, 20, "%04d", pot);

        //_delay_ms(50);

        // run drive menu
        DM_process();

        Present_Buffer();
    }
}