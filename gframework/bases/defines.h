#pragma once

#ifdef __AVR__
#include <avr/pgmspace.h>
// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif
#else
#define PROGMEM
#define xpgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif