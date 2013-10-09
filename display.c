#include <msp430.h>
#include <stdint.h>
#include "display.h"

#define COMMON_ANODE

#ifdef COMMON_ANODE
  #define LED_CONFIG(x)     ( (uint8_t)(~(x)) )
  #define SEGMENT(x)        ( segments[x])
#else
  #define LED_CONFIG(x)     ( (uint8_t)((x)) )
  #define SEGMENTS(x)       ( ~segments[x] )
#endif

#define DASH                (10)

static const uint8_t segments[11] = {
    LED_CONFIG(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT7),          // 0
    LED_CONFIG(BIT4 | BIT7),                                      // 1
    LED_CONFIG(BIT0 | BIT4 | BIT5 | BIT2 | BIT3),                 // 2
    LED_CONFIG(BIT0 | BIT4 | BIT5 | BIT7 | BIT3),                 // 3
    LED_CONFIG(BIT1 | BIT4 | BIT5 | BIT7),                        // 4
    LED_CONFIG(BIT0 | BIT1 | BIT5 | BIT7 | BIT3),                 // 5
    LED_CONFIG(BIT1 | BIT2 | BIT5 | BIT7 | BIT3),                 // 6
    LED_CONFIG(BIT0 | BIT4 | BIT7),                               // 7
    LED_CONFIG(BIT0 | BIT1 | BIT4 | BIT5 | BIT2 | BIT3 | BIT7),   // 8
    LED_CONFIG(BIT0 | BIT1 | BIT4 | BIT5 | BIT7),                 // 9
    LED_CONFIG(BIT5)                                              // - (10)
};

void display_init(void)
{
  P2DIR   = 0xFF;
  P2OUT   = 0xFF;
  P2SEL   = 0;
  P2SEL2  = 0;
}

void display_write(const int val)
{
  if(val >= 0 && val <= 9) {
    P2OUT = SEGMENT(val);
  } else {
    P2OUT = SEGMENT(DASH);
  }
}

void display_on(const int val)
{
  display_write(val);
}

void display_off(void)
{
  P2OUT = 0xff;
}
