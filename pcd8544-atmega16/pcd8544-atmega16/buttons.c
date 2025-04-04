#include "buttons.h"
#include <avr/io.h>
#include <util/delay.h>

#define SH_LD PD2
#define CLK   PD3
#define QH    PD4

void shift_register_init() {
	DDRD |= (1 << SH_LD) | (1 << CLK);
	DDRD &= ~(1 << QH);
	PORTD |= (1 << SH_LD);
	PORTD &= ~(1 << CLK);
	_delay_us(5);
}

uint8_t read_buttons() {
	uint8_t data = 0;
	PORTD &= ~(1 << SH_LD);
	_delay_us(5);
	PORTD |= (1 << SH_LD);
	_delay_us(5);

	for (uint8_t i = 0; i < 8; i++) {
		data <<= 1;
		if (PIND & (1 << QH)) {
			data |= 1;
		}
		PORTD |= (1 << CLK);
		_delay_us(2);
		PORTD &= ~(1 << CLK);
		_delay_us(2);
	}
	return data;
}