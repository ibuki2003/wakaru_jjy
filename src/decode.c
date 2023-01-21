#include "decode.h"

#include <avr/interrupt.h>
#include "jjy.h"
#include "lcd.h"

uint8_t buf[16] = {0};

uint8_t csr = 0xfe;

uint8_t last_state = 0;

const uint8_t WDAYS[] =
  "SuMoTuWeThFrSa";

const uint8_t hex[] = "0123456789abcdef";

const uint8_t RCV_STAT_CHARS[] = "-ox";

const uint8_t CURSOR_POS[] = {
  //yydddwhhmmlpr
  0x00, 0x01, 0x09, 0x0a, 0x0b, 0x0e,
  0x40, 0x41, 0x43, 0x44, 0x4b, 0x4e,
  0x20, 0x20, 0x20, 0x20,
};

/**
  update LCD

  preview:

  - PLAN A:
    > 23-01-12(012)th
    > 14:17:20 (L+) .o
  yydddwhhmmlpr
  0123456789abc
*/
inline static void draw_state() {

  lcdwrite(0, 0, 0x80); // set DDRAM address to 0x00

  lcdwrite(1, 0, '0' + buf[0]); // year
  lcdwrite(1, 0, '0' + buf[1]);
  lcdwrite(1, 0, '-');
  lcdwrite(1, 0, '0'); // month; assume Janurary
  lcdwrite(1, 0, '1');
  lcdwrite(1, 0, '-');
  lcdwrite(1, 0, '0' + buf[3]); // day; assume Janurary :)
  lcdwrite(1, 0, '0' + buf[4]);
  lcdwrite(1, 0, '(');
  lcdwrite(1, 0, '0' + buf[2]);
  lcdwrite(1, 0, '0' + buf[3]);
  lcdwrite(1, 0, '0' + buf[4]);
  lcdwrite(1, 0, ')');
  lcdwrite(1, 0, ' ');
  lcdwrite(1, 0, WDAYS[buf[5]*2]); // weekday
  lcdwrite(1, 0, WDAYS[buf[5]*2+1]); // weekday
  /* lcdwrite(1, 0, '0' + buf[5]); // weekday */

  lcdwrite(0, 0, 0xc0); // set DDRAM address to 0x40
  lcdwrite(1, 0, '0' + buf[6]);
  lcdwrite(1, 0, '0' + buf[7]);
  lcdwrite(1, 0, ':');
  lcdwrite(1, 0, '0' + buf[8]);
  lcdwrite(1, 0, '0' + buf[9]);
  lcdwrite(1, 0, ':');
  if (csr & 0x80) {
    // not syncing
    lcdwrite(1, 0, '-');
    lcdwrite(1, 0, '-');
  } else {
    lcdwrite(1, 0, '0' + csr / 10);
    lcdwrite(1, 0, '0' + csr % 10);
  }

  lcdwrite(1, 0, ' ');
  lcdwrite(1, 0, ' ');
  lcdwrite(1, 0, 'L');
  if (buf[10] & 0b10) {
    lcdwrite(1, 0, '0');
  } else {
    if (buf[10] & 0b01) {
      lcdwrite(1, 0, '+');
    } else {
      lcdwrite(1, 0, '-');
    }
  }

  lcdwrite(1, 0, ' ');
  lcdwrite(1, 0, (csr & 0x80) ? '-' : '+'); // sync status
  lcdwrite(1, 0, RCV_STAT_CHARS[last_state]); // receive status

  if (csr & 0x80) {
    lcdwrite(0, 0, 0b00001100); // cursor off
  } else {
    lcdwrite(0, 0, 0b00001111); // cursor on
    lcdwrite(0, 0, 0x80 | CURSOR_POS[JJY_TABLE[csr] >> 4]);
    lcdwrite(0, 0, 0b00010100); // cursor move
  }
}

ISR(TIMER0_OVF_vect) {
  draw_state();
}
