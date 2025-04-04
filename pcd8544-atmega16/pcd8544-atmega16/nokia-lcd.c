#include "nokia-lcd.h"
#include "font.h"

#include <util/delay.h>

void pcd8544_init(void) {
	// Настройка GPIO
	DDRB |= (1 << PIN_RESET) | (1 << PIN_DC) | (1 << PIN_SCE) | (1 << PIN_SDIN) | (1 << PIN_SCLK);

	// Настройка SPI (Master mode, Fosc/16)
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

	// Сброс дисплея
	PORTB &= ~(1 << PIN_RESET);
	_delay_ms(100);
	PORTB |= (1 << PIN_RESET);

	// Инициализация дисплея
	pcd8544_send(LCD_C, 0x21);
	pcd8544_send(LCD_C, 0xBA);
	pcd8544_send(LCD_C, 0x04);
	pcd8544_send(LCD_C, 0x14);
	pcd8544_send(LCD_C, 0x20);
	pcd8544_send(LCD_C, 0x0C);
}

void pcd8544_send(uint8_t dc, uint8_t data) {
	if (dc == LCD_D)
	PORTB |= (1 << PIN_DC);
	else
	PORTB &= ~(1 << PIN_DC);

	PORTB &= ~(1 << PIN_SCE);
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	PORTB |= (1 << PIN_SCE);
}

void pcd8544_print_string(char *str) {
	while (*str) {
		pcd8544_send_char(*str++);
	}
}

void pcd8544_send_char(uint8_t ch) {
	int i;
	if (ch >= 0x20 && ch <= 0x80) {
		pcd8544_send(LCD_D, 0x00);
		for (i = 0; i < 5; i++) {
			pcd8544_send(LCD_D, ASCII[ch - 0x20][i]);
		}
		pcd8544_send(LCD_D, 0x00);
	}
}

void pcd8544_clear(void) {
	for (int i = 0; i < LCD_X * LCD_Y / 8; i++) {
		pcd8544_send(LCD_D, 0x00);
	}
}

void pcd8544_set_cursor(uint8_t x, uint8_t y) {
	x = x % 12; y = y % 6;
	pcd8544_send(LCD_C, 0x40 + y);
	pcd8544_send(LCD_C, 0x80 + x * 7);
}

void pcd8544_print_at(char *str, uint8_t x, uint8_t y) {
	pcd8544_set_cursor(x, y);
	while (*str) {
		pcd8544_send_char(*str++);
	}
}
