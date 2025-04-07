#include <avr/io.h>
#include <util/delay.h>

#define PIN_RESET PB2
#define PIN_DC    PB3
#define PIN_SCE   PB4
#define PIN_SDIN  PB5
#define PIN_SCLK  PB7

#define LCD_C     0x00
#define LCD_D     0x01

const uint8_t HELLO[][5] = {
	{0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
	{0x38, 0x54, 0x54, 0x54, 0x18}, // e
	{0x00, 0x41, 0x7F, 0x40, 0x00}, // l
	{0x00, 0x41, 0x7F, 0x40, 0x00}, // l
	{0x38, 0x44, 0x44, 0x44, 0x38}  // o
};

void send(uint8_t dc, uint8_t data) {
	if (dc)
	PORTB |= (1 << PIN_DC);
	else
	PORTB &= ~(1 << PIN_DC);

	PORTB &= ~(1 << PIN_SCE);

	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80)
		PORTB |= (1 << PIN_SDIN);
		else
		PORTB &= ~(1 << PIN_SDIN);

		PORTB |= (1 << PIN_SCLK);
		data <<= 1;
		PORTB &= ~(1 << PIN_SCLK);
	}

	PORTB |= (1 << PIN_SCE);
}

int main(void) {
	// Настройка пинов
	DDRB |= (1 << PIN_RESET) | (1 << PIN_DC) | (1 << PIN_SCE) | (1 << PIN_SDIN) | (1 << PIN_SCLK);

	// Сброс дисплея
	PORTB &= ~(1 << PIN_RESET);
	_delay_ms(10);
	PORTB |= (1 << PIN_RESET);

	// Инициализация дисплея
	send(LCD_C, 0x21); // Расширенный режим
	send(LCD_C, 0xB1); // Контраст
	send(LCD_C, 0x04); // Температурный коэффициент
	send(LCD_C, 0x14); // Bias
	send(LCD_C, 0x20); // Базовый режим
	send(LCD_C, 0x0C); // Нормальный режим

	// Очистка экрана
	for (uint16_t i = 0; i < 504; i++) {
		send(LCD_D, 0x00);
	}

	// Печать "Hello"
	for (uint8_t i = 0; i < 5; i++) {
		for (uint8_t j = 0; j < 5; j++) {
			send(LCD_D, HELLO[i][j]);
		}
		send(LCD_D, 0x00); // Пробел между буквами
	}

	while (1);
}
