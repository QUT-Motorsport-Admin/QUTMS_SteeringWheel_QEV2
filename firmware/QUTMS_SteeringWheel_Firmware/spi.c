/*****************************************************************************
* @file    firmware/QUTMS_SteeringWheel_Firmware/spi.c
* @author  Zoe Goodward
* @version V1.0.0
* @date    2/08/2019 1:25:34 PM
* @brief   This file...
*****************************************************************************/

/* INCLUDES */
#include <inttypes.h>
#include <avr/io.h>
#include "spi.h"
#include "OLED.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  SPI Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void spi_init(void)
{
	SPCR = (SPI_INTERRUPT<<SPIE)
		|(1<<SPE)
		|(SPI_DATA_ORDER<<DORD)	
		|(SPI_MSTR_MODE<<MSTR)
		|(SPI_CLK_POLARITY<<CPOL)
		|(SPI_CLK_PHASE<<CPHA)
		|(SPI_CLK_RATE0<<SPR0)
		|(SPI_CLK_RATE1<<SPR1);
	//set interrupt, enable SPI, MSB mode, set as master, low is idle, read on leading edge, Set to speed(fosc/4) SPR0=1, SPR1=0
	SPSR = (DBL_CLK<<SPI2X);	//set whether we want 2x speed or not (1=2x speed).
	SPDR = 0x00;		//ensure data register has nothing in it
}




//shifts out 8 bits of data
//  uint8_t data - the data to be shifted out
//  returns uint8_t - the data received during sending
uint8_t spi_send_byte(uint8_t data) {
	/* Start transmission */ // Transmission is started by writing data to the transmission register
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	/* Return any data that was shifted into this register upon transmission  */
	return SPDR;
}

void spi_disable(void) {
	SPDR = 0;
}

/*
Only works when compiled with -O1 or no optimization

// example usage: spi_init(SPI_INTERRUPT_DISABLED, SPI_MSB, SPI_MASTER, SPI_MODE_0, SPI_CLOCK_RATE_4);
void spi_init(uint8_t intEnable, uint8_t dataOrder, uint8_t mstrMode, uint8_t spiMode, uint8_t clkRate) {
	// Master or slave
	if (mstrMode == SPI_MASTER) { // Master
		// Set MOSI and SCK output, all others input
		DDRB |= (1<<SPI_MOSI_PIN)|(1<<SPI_SCK_PIN); // Output
		DDRB &= ~(1<<SPI_MISO_PIN); // Input
	} else {
		// Set MISO output, all others input
		DDRB |= (1<<SPI_MISO_PIN); // Output
		DDRB &= ~(1<<SPI_MOSI_PIN)|(1<<SPI_SCK_PIN); // Input
	}
	SPCR = (intEnable << SPIE) // interrupt
		 | (1<<SPE) // Enable SPI
		 | (dataOrder << DORD) // LSB or MSB
		 | (mstrMode << MSTR) // Master or Slave mode
		 | (((spiMode & SPI_CPOL_MASK) == 2) << CPOL) // Clock timing mode CPOL
		 | ((spiMode & SPI_CPHA_MASK) << CPHA) // Clock timing mode CPHA
		 | (((clkRate & SPI_SPR1_MASK) == 2) << SPR1) // CPU clock divisor SPR1
		 | ((clkRate & SPI_SPR0_MASK) << SPR0); // CPU clock divisor SPR0
		 
	SPSR = (((clkRate & SPI_DBCLK_MASK) == 4) << SPI2X); // Set whether we want 2x speed or not
	SPDR = 0x00;
}
*/