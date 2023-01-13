#pragma once
#include <stdint.h>

/**
  JJY signal "protocol" data

  each value represents seconds

  - 0xf2: marker, 0.2s pulse
  - 0xf0: constantly '0'
  - else:
    upper 4 bits: byte position of data
    lower 4 bits: bit position of data (i.e. bit mask)

  yydddwhhmmlpr
  0123456789abc
  */
extern const uint8_t JJY_TABLE[];
