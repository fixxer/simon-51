#include <at89x051.h>

#define ASK 0
#define ANSWER 1
#define BUTTONS_COUNT 4
#define CLOCK 6000000
#define TIMER_SCALE_FACTOR 12

const unsigned char leds[] = {0b10011111,  0b01011111, 0b00111111, 0b11111111};
const unsigned char button_masks[] = {0b00010000,  0b00001000, 0b00000100, 0b00000010};
volatile unsigned int millis = 0;
volatile unsigned char countdowns[] = {0, 0}; 
volatile __sbit half_millis_counter = 0;
volatile unsigned char tone_timer_hi = 0;
volatile unsigned char tone_timer_lo = 0;

/**
568 - 880 Hz
*/
void tone(unsigned int divider, unsigned char duration_ms) {
  unsigned int timer_init = 0xffff - divider;
  tone_timer_hi = (unsigned char) (timer_init >> 8);
  tone_timer_lo = (unsigned char) timer_init;
  TH0 = tone_timer_hi;
  TL0 = tone_timer_lo;
  countdowns[0] = duration_ms;
  TR0 = 1;
}

void main() {
  unsigned char mode = ASK;
  unsigned char current_led = 0;

  unsigned char button_flags[] = {0, 0, 0, 0};

  // Setup timers
  TH1 = 0x06;  // Set timer1 value (256 - 6 = 250 count until trigger)
  TL1 = 0x06;
  TMOD = 0x21; // Set timer0 mod1, timer1 mod2 
  TR1 = 1; // Start timer1
  ET0 = 1; // Enable timer0 interrupt
  ET1 = 1; // Enable timer1 interrupt
  EA = 1;  // Enable interrupts

  while(1) {
    if (mode == ASK) {
      current_led = TL1 & 0x03; // Use timer counter as a pseudorandom
      P1 = leds[current_led];
      mode = ANSWER;
      tone(568, 255);
    } else {
      for (unsigned char i = 0; i < BUTTONS_COUNT; i++) {
        if (!(P1 & button_masks[i]) && button_flags[i] == 0 && countdowns[1] == 0) {
          countdowns[1] = 50;
          button_flags[i] = 1;          
          if (i == current_led) {
            //tone(568, 255);
            tone(568 / 2, 255);
          } else {
            //tone(568, 255);
            tone(568 * 2, 255);
          }
        } else if (P1 & button_masks[i] && button_flags[i] == 1 && countdowns[1] == 0) {
          countdowns[1] = 50;
          button_flags[i] = 0;          
          mode = ASK;
        }
      }
    }
  }
}

void timer0() __interrupt (TF0_VECTOR) {
  TR0 = 0; //Disable timer0

  P3_7 ^= 1; // Toggle pin

  TH0 = tone_timer_hi; // Reset timer0 value
  TL0 = tone_timer_lo;

  if (countdowns[0] > 0) {
    TR0 = 1; // Enable timer0
  } else {
    P3_7 = 0; // Sound off
  }
}

void timer1() __interrupt (TF1_VECTOR) {
  half_millis_counter ^= 1;
  if (half_millis_counter == 0) {
    if (countdowns[0] > 0) countdowns[0]--;
    if (countdowns[1] > 0) countdowns[1]--;
    millis++;
  }
}