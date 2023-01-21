#pragma once
#include "config.h"

#include <stdint.h>

#include "jjy.h"

extern uint8_t buf[16];

extern uint8_t csr; // position of JJY_TABLE (i.e. current second). sync steps: 0xfe -> 0xff -> 0x00

/*
  last receive result
  0: no receive
  1: receive success
  2: receive error
*/
extern uint8_t last_state;

/*
   update buf and some states.

   @param data: 1 tick data
      0: 0 (0.8s pulse)
      1: 1 (0.5s pulse)
      2: marker (0.2s pulse)
*/
static inline void put_data(uint8_t data) {
  if (csr & 0x80) {
    // not syncing
    if (data == 2) {
      // got marker
      // wait for 59, 00 markers
      ++csr;
    } else {
      csr = 0xfe;
    }
  } else {
    uint8_t p = JJY_TABLE[csr] >> 4;
    uint8_t q = JJY_TABLE[csr] & 0x0f;

    if (p == 0xf) {
      if (data != q) {
        // fail
        csr = 0xfe;
        last_state = 2;
      } else {
        ++csr;
      }
    } else {
      if (data == 2) {
        // fail
        csr = 0xfe;
        last_state = 2;
      } else {
        buf[p] = buf[p] | (data << q);
        ++csr;
        uint8_t r = JJY_TABLE[csr] >> 4;
        if (p != r && r != 0xf) {
          buf[r] = 0;
        }
      }
    }
    if (csr == 59) {
      csr = 0xff;

      // TODO: validate checksum

      last_state = 1;
    }
  }
}

