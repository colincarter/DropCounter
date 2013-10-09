#include <msp430.h>
#include <stdint.h>
#include "adxl362_defs.h"
#include "spi.h"
#include "display.h"

#define LED   BIT0
#define INT1  BIT3  // Accelerometer wakeup interrupt 1
#define CS    BIT4  // Accelerometer Chip Select


#define WAKE_NONE     (uint16_t)(0)
#define WAKE_ACCEL    (uint16_t)(1L << 0)

static void init_hardware(void);
static void setup_system(void);
static void accel_write(uint8_t address, uint8_t data);
//static uint8_t accel_read(uint8_t address);
static void csl(void);
static void csh(void);
static uint16_t get_wake_source(void);
static void clear_wake_source_bit(uint16_t bit);

static uint16_t drop_count;
static volatile uint16_t wake_source;

int main(void)
{
  init_hardware();
  setup_system();

  while(1) {
    _BIS_SR(LPM4_bits + GIE);

    if(get_wake_source() & WAKE_ACCEL) {
      clear_wake_source_bit(WAKE_ACCEL);
      P1OUT ^= LED;
      drop_count++;
    }
  }

	//return 0;
}

static void init_hardware(void)
{
  WDTCTL = WDTPW + WDTHOLD;

  // Init clock to 1MHZ
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
}

static void setup_system(void)
{
  P1DIR = 0xff;
  P2DIR = 0xff;
  P3DIR = 0xff;

  P1DIR |= (LED | CS);
  P1DIR &= ~INT1;
  P1IE  |= INT1;
  P1IES &= ~INT1; // Low to high transition
  P1IFG &= ~INT1; // Clear interrupt flag

  P1OUT &= ~LED;

  display_init();

  drop_count = 0;
  wake_source = WAKE_NONE;

  csh();

  spi_init();

  accel_write(ADXL362_SOFT_RESET, 0);
  __delay_cycles(75);

  //
  // The accelerometer sets the INT1 pin when a 0g
  // inactivity (free fall) is detected.
  //
  accel_write(ADXL362_THRESH_INACT_L, 0x96);
  accel_write(ADXL362_TIME_INACT_L, 0x03);
  accel_write(ADXL362_ACT_INACT_CTL, 0x0c);
  accel_write(ADXL362_INTMAP1, 0x20);
  accel_write(ADXL362_FILTER_CTL, 0x83);
  accel_write(ADXL362_POWER_CTL, 0x02);

}

static void accel_write(uint8_t address, uint8_t data)
{
  csl();
  spi_transfer(ADXL362_REG_WRITE);
  spi_transfer(address);
  spi_transfer(data);
  csh();
}

#if 0
static uint8_t accel_read(uint8_t address)
{
  uint8_t result;

  csl();
  spi_transfer(ADXL362_REG_READ);
  spi_transfer(address);
  result = spi_transfer(0xFF);
  csh();

  return result;
}
#endif

static void csl(void)
{
  P1OUT &= ~CS;
}

static void csh(void)
{
  P1OUT |= CS;
}

static uint16_t get_wake_source(void)
{
  uint16_t source;
  __disable_interrupt();
  source = wake_source;
  __enable_interrupt();
  return source;
}

static void clear_wake_source_bit(uint16_t bit)
{
  __disable_interrupt();
  wake_source &= ~bit;
  __enable_interrupt();
}

#pragma vector=PORT1_VECTOR
__interrupt void port1_vector_interrupt(void)
{
  if(P1IFG & INT1) {
    wake_source |= WAKE_ACCEL;
    P1IFG &= ~INT1;
  }

  _BIC_SR_IRQ(LPM4_bits);
}

#pragma vector=PORT2_VECTOR
#pragma vector=TIMER1_A1_VECTOR
#pragma vector=TIMER0_A1_VECTOR
#pragma vector=TIMER0_A0_VECTOR
#pragma vector=WDT_VECTOR
#pragma vector=ADC10_VECTOR
#pragma vector=COMPARATORA_VECTOR
#pragma vector=TIMER1_A0_VECTOR
#pragma vector=NMI_VECTOR
#pragma vector=USCIAB0TX_VECTOR
#pragma vector=USCIAB0RX_VECTOR
__interrupt void isr_trap(void)
{
  WDTCTL = 0;
}
