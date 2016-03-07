#include <msp430.h>
#include <stdint.h>
#include "adxl362_defs.h"
#include "spi.h"
#include "display.h"

#define DEBUG
#define WAKE_ON_SHAKE

#if !defined(WAKE_ON_SHAKE) && !defined(WAKE_ON_FALL)
#error "WAKE_ON_SHAKE or WAKE_ON_FALL not defined"
#endif

#if defined(DEBUG)
#define LED   BIT0
#endif

#define INT1  BIT1  // Accelerometer wakeup interrupt 1
#define CS    BIT4  // Accelerometer Chip Select

#define WAKE_NONE     (uint16_t)(0)
#define WAKE_ACCEL    (uint16_t)(1L << 0)

#define HIGH_BYTE(x) (uint8_t)((x>>8)&0xff)
#define LOW_BYTE(x)  (uint8_t)(x&0xff)

static void init_hardware(void);
static void setup_system(void);
static void accel_write(uint8_t address, uint8_t data);
static inline void csl(void);
static inline void csh(void);
static uint16_t get_wake_source(void);
static void clear_wake_source_bit(uint16_t bit);

#if defined(WAKE_ON_SHAKE)
static uint8_t accel_read(uint8_t address);
static void accel_activity_interrupt(uint16_t threshold, uint8_t time);
static void accel_inactivity_interrupt(uint16_t threshold, uint16_t time);
static void accel_write_two(uint8_t address, uint16_t data);
static void accel_begin_measure(void);
#endif

#if defined(DEBUG)
static inline void debug_led_off(void);
static inline void debug_led_on(void);
#endif

#if defined(WAKE_ON_FALL)
static uint16_t drop_count;
#endif

static volatile uint16_t wake_source;

int main(void)
{
    init_hardware();
    setup_system();

    while(1)
    {
        LPM4;

        if(get_wake_source() & WAKE_ACCEL)
        {
            clear_wake_source_bit(WAKE_ACCEL);

#if defined(WAKE_ON_FALL)
            drop_count++;
#endif

#if defined(DEBUG)
            debug_led_on();
#endif
        }
    }
}

static void init_hardware(void)
{
    WDTCTL = WDTPW + WDTHOLD;

    // Init clock to 1MHZ
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1DIR = 0xff;
    P2DIR = 0xff;
    P3DIR = 0xff;
    P1OUT = 0x00;
    P2OUT = 0x00;
    P3OUT = 0x00;
}

static void setup_system(void)
{
    P1DIR |= CS;

#if defined(DEBUG)
    P1DIR |= LED;
#endif

    P1DIR &= ~INT1;
    P1IE  |= INT1;
    P1IES &= ~INT1; // Low to high transition
    P1IFG &= ~INT1; // Clear interrupt flag

#if defined(DEBUG)
    debug_led_off();
#endif

    display_init();

#if defined(WAKE_ON_FALL)
    drop_count = 0;
#endif

    wake_source = WAKE_NONE;

    csh();

    spi_init();

    accel_write(ADXL362_SOFT_RESET, 0);
    __delay_cycles(75);

#if defined(WAKE_ON_SHAKE)
    accel_write(ADXL362_FILTER_CTL, 0x11);
    accel_activity_interrupt(300, 200);
    accel_inactivity_interrupt(80, 50);
    accel_write(0x2A, 0x40);
    accel_write(0x27, 0xFF);
    uint8_t reg = accel_read(0x2D);
    reg |= (0x04);
    accel_write(0x2D, reg);
    accel_begin_measure();
#elif defined(WAKE_ON_FALL)
    //
    // The accelerometer sets the INT1 pin when a 0g
    // inactivity (free fall) is detected.
    //
    accel_write(ADXL362_THRESH_INACT_L, 0x96);
    accel_write(ADXL362_TIME_INACT_L, 0x03); // 0x03
    accel_write(ADXL362_ACT_INACT_CTL, 0x0c);
    accel_write(ADXL362_INTMAP1, 0x20);
    accel_write(ADXL362_FILTER_CTL, 0x83);// 0x83
    accel_write(ADXL362_POWER_CTL, 0x02);
#endif

    __enable_interrupt();
}

#if defined(WAKE_ON_SHAKE)
void accel_begin_measure(void)
{
    uint8_t reg;

    reg = accel_read(0x2D);
    reg |= 0x02;
    accel_write(0x2D, reg);
}
#endif

static void accel_write(uint8_t address, uint8_t data)
{
    csl();
    spi_transfer(ADXL362_REG_WRITE);
    spi_transfer(address);
    spi_transfer(data);
    csh();
}

#if defined(WAKE_ON_SHAKE)
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

static void accel_write_two(uint8_t address, uint16_t data)
{
    csl();
    spi_transfer(ADXL362_REG_WRITE);
    spi_transfer(address);
    spi_transfer(LOW_BYTE(data));
    spi_transfer(HIGH_BYTE(data));
    csh();
}

static void accel_activity_interrupt(uint16_t threshold, uint8_t time)
{
    uint8_t reg;

    accel_write_two(0x20, threshold);
    accel_write(0x22, time);

    reg = accel_read(0x27);
    reg |= 0x01;
    accel_write(0x27, reg);
}

static void accel_inactivity_interrupt(uint16_t threshold, uint16_t time)
{
    uint8_t reg;

    accel_write_two(0x23, threshold);
    accel_write_two(0x25, time);

    reg = accel_read(0x27);
    reg |= 0x04;
    accel_write(0x27, reg);
}
#endif

static inline void csl(void)
{
    P1OUT &= ~CS;
}

static inline void csh(void)
{
    P1OUT |= CS;
}

#if defined(DEBUG)
static inline void debug_led_on(void)
{
    P1OUT |= LED;
}


static inline void debug_led_off(void)
{
    P1OUT &= ~LED;
}
#endif

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
    if(P1IFG & INT1)
    {
        wake_source |= WAKE_ACCEL;
        P1IFG &= ~INT1;
    }

    LPM4_EXIT;
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
