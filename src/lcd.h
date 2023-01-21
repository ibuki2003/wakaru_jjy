#pragma once
#include "config.h"

#include <avr/io.h>
#include <util/delay.h>

void _lcdwrite(uint8_t rs_rw, uint8_t data);

#define lcdwrite(rs, rw, data) _lcdwrite((rw << 1) | (rs << 2), data)

