#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "pcd8544.h"  // Дисплей

// Подключение 74HC165 к PD2 (PL), PD3 (CLK), PD4 (QH)
#define SH_LD PD2
#define CLK   PD3
#define QH    PD4

void shift_register_init() {
	// PL и CLK — выходы, QH — вход
	DDRD |= (1 << SH_LD) | (1 << CLK);
	DDRD &= ~(1 << QH);

	// Установим линии в исходное состояние
	PORTD |= (1 << SH_LD);  // ~SH_LD = 1 (режим сдвига)
	PORTD &= ~(1 << CLK);   // CLK = 0
}

uint8_t read_buttons() {
	uint8_t data = 0;

	// Загружаем данные с кнопок во внутренний регистр
	PORTD &= ~(1 << SH_LD);   // ~SH_LD = 0 (загрузка)
	_delay_us(5);
	PORTD |= (1 << SH_LD);    // ~SH_LD = 1 (режим сдвига)
	_delay_us(5);

	// Считываем 8 бит
	for (uint8_t i = 0; i < 8; i++) {
		data <<= 1;
		if (PIND & (1 << QH)) {
			data |= 1;
		}

		// Импульс на тактовой линии
		PORTD |= (1 << CLK);
		_delay_us(2);
		PORTD &= ~(1 << CLK);
		_delay_us(2);
	}

	return data;
}

int main(void) {
	pcd8544_init();
	pcd8544_clear();

	shift_register_init();

	char buffer[16];

	while (1) {
		uint8_t btns = read_buttons();

		if (btns != 0xFF) {  // Если хоть одна кнопка нажата (низкий уровень)
			for (uint8_t i = 0; i < 8; i++) {
				if (!(btns & (1 << (7 - i)))) { // Активный уровень – 0
					pcd8544_clear();
					sprintf(buffer, "Button: %d", i + 1);
					pcd8544_print_at(buffer, 1, 1);
					break;
				}
			}
		}

		_delay_ms(100);  // Защита от дребезга
	}

	return 0;
}
