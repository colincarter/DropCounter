#include <msp430.h>
#include <stdint.h>
#include "spi.h"

#define SCLK  BIT5  // SCK
#define SOMI  BIT6  // MISO
#define SIMO  BIT7  // MOSI

void spi_init(void)
{
  P1SEL  |= SOMI + SIMO + SCLK;
  P1SEL2 |= SOMI + SIMO + SCLK;

  // Setup SPI
  UCB0CTL1 |= UCSWRST;
  UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;
  UCB0CTL1 |= UCSSEL_2;
  UCB0BR0 |= 0x02;
  UCB0BR1 = 0;
  UCB0CTL1 &= ~UCSWRST;
}

uint8_t spi_transfer(uint8_t data)
{
  while (!(UC0IFG & UCB0TXIFG)) {
  }

  UCB0TXBUF = data;

  while (!(UC0IFG & UCB0RXIFG)) {
  }

  return UCB0RXBUF;
}
