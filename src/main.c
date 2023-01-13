#define F_CPU 8000000UL // 8MHz
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "lcd.h"
#include "decode.h"

static inline void init() {
  // ports
  DDRB  = 0b11111111;
  PORTB = 0x00;

  DDRD  = 1<<6;
  PORTD = 0x00;

  DDRA  = 3<<0;
  PORTA = 0x00;

  // LCD setup
  _delay_ms(100);

  lcdwrite(0, 0, 0b00111000); // init
  for (int i = 0; i < 3; ++i) {
    _delay_ms(100);
    PORTD = 1<<6; _delay_us(1); PORTD = 0;
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

  // set overflow handler: 16bit timer #1
  TCCR1A = 0;
  TCCR1B = 0b101; // 1024 prescaler

  // timer2 init (LCD)
  // set handler
  TCCR0A = 0;
  TCCR0B = 0b101; // 1024 prescaler

  TIMSK = 1<<TOIE0; // enable overflow interrupt
  // handler is in decode.c

  bool last = 0;
  while(1) {
    if ((PORTA & 1) == last) continue;
    // toggle found

    last = !last;

    if (last) {
      // raise

      // check timer
      if ((TIFR & (1 << TOV1)) || (TCNT1H >= 100)) {
        // timeout
        last_state = 2;
        csr = 0xfe;
      }
      // timer reset
      TCNT1 = 0;
      TIFR = 1<<TOV1; // clear overflow flag
    } else {
      // fall
      if (TIFR & (1<<TOV1) || (TCNT1H >= 100)) {
        // timeout
        last_state = 2;
        csr = 0xfe;
      } else {
        // check timer
        if (TCNT1H < 10)
          // marker
          put_data(2);
        else if (TCNT1H < 20)
          // short pulse; 1
          put_data(1);
        else
          // long pulse; 0
          put_data(0);
      }
    }

  }
}
