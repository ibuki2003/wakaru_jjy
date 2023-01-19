#define F_CPU 8000000UL // 8MHz
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "lcd.h"
#include "decode.h"

#define TIMER_FREQ (F_CPU / 1024 / 256) // higher byte
// thresholds
#define TIMER1_1100MS (TIMER_FREQ * 11 / 10)
#define TIMER1_600MS  (TIMER_FREQ * 6 / 10)
#define TIMER1_300MS  (TIMER_FREQ * 3 / 10)

static inline void init() {
  // ports

  // PD: LCD data
  DDRD  = 0b11111111;
  PORTD = 0x00;

  // PC: LCD control
  DDRC  = 0b111;
  PORTC = ~0b111; // pull up

  // PB: input and PWM output
  DDRB  = 0b00001000;
  PORTB = ~0b00001000; // pull up

  // LCD setup
  _delay_ms(100);

  lcdwrite(0, 0, 0b00111000); // init
  for (int i = 0; i < 3; ++i) {
    _delay_ms(100);
    PORTC |= 0b001; _delay_us(1); PINC = 0b001;
  }

  lcdwrite(0, 0, 0b00111000); // function set, 8bit, 2line, 5x11
  lcdwrite(0, 0, 0b00001100); // display, on, cursor off
  lcdwrite(0, 0, 0b00000001); // clear display
  lcdwrite(0, 0, 0b00000110); // entry mode, increment, no shift

  _delay_ms(100);
}

const uint8_t SPLASH_TEXT[] =
  "   wakaru JJY   "
  "    ｢ﾜｯｶﾙｰ!｣    ";

int main() {
  init();

  // splash screen
  for (uint8_t i = 0; i < 16; ++i) {
    lcdwrite(1, 0, SPLASH_TEXT[i]);
  }
  lcdwrite(0, 0, 0x80 | 0x40); // 2nd row
  for (uint8_t i = 0; i < 16; ++i) {
    lcdwrite(1, 0, SPLASH_TEXT[i+16]);
  }
  _delay_ms(2000);

  // timer init

  // timer0 init (LCD)
  // set handler
  TCCR0 = 0b101; // 1024 prescaler

  // enable overflow interrupt
  // NOTE: handler is in decode.c
  TIMSK = 1<<TOIE0;

  // 16bit timer #1
  TCCR1A = 0;
  TCCR1B = 0b101; // 1024 prescaler


  uint8_t last = 0;
  while(1) {
    if ((PINB & 2) == last) continue;
    // toggle found

    last ^= 2;

    if ((TIFR & (1 << TOV1)) || (TCNT1H > TIMER1_1100MS)) {
      // unsync anyway if timeover
      last_state = 2;
      csr = 0xfe;
    }

    if (last) {
      // raise

      // timer reset
      TCNT1 = 0;
      TIFR = 1<<TOV1; // clear overflow flag
    } else {
      // fall
      if (TCNT1H < TIMER1_300MS)
        // marker
        put_data(2);
      else if (TCNT1H < TIMER1_600MS)
        // short pulse; 1
        put_data(1);
      else
        // long pulse; 0
        put_data(0);
    }

  }
}
