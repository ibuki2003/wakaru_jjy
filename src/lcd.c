#include "lcd.h"

void _lcdwrite(uint8_t rs_rw, uint8_t data) {
  PORTC = rs_rw;
  PORTD = data;
  PORTC |= 0b100; // E up
  PINC = 0b100; // E down (toggle)
  _delay_us(100);
}
