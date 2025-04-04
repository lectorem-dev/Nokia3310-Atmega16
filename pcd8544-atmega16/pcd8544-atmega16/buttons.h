#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

void shift_register_init(void);
uint8_t read_buttons(void);

#endif