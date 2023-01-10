#include "lcd.h"

void _lcdwrite(uint8_t rs_rw, uint8_t data) {
  PORTA = rs_rw;
  PORTB = data;
  PORTD = 1<<6; _delay_us(1); PORTD = 0; _delay_us(100);
}
