#include "pcd8544.h"
#include "pcd8544_font.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#define SH_LD PD2
#define CLK   PD3
#define QH    PD4

// ������������� ���������� ��������
void shift_register_init() {
	DDRD |= (1 << SH_LD) | (1 << CLK);
	DDRD &= ~(1 << QH);
	PORTD |= (1 << SH_LD);
	PORTD &= ~(1 << CLK);
}

// ������ ��������� ������
uint8_t read_buttons() {
	uint8_t data = 0;
	PORTD &= ~(1 << SH_LD);   // �������� ������ � ������
	_delay_us(5);
	PORTD |= (1 << SH_LD);    // ������������ � ����� ������
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

int main(void) {
	pcd8544_init();
	pcd8544_clear();
	shift_register_init();

	uint8_t x = 3, y = 2; // ��������� ������� �����
	char buffer[16];

	while (1) {
		uint8_t btns = read_buttons();

		// ���� ������ 1 (�����) ������
		if (!(btns & (1 << 0)) && y > 0) {
			y--;
			pcd8544_clear();
			sprintf(buffer, "X: %d Y: %d", x, y);
			pcd8544_print_at(buffer, 0, 0);
		}
		// ���� ������ 2 (����) ������
		else if (!(btns & (1 << 1)) && y < 5) {
			y++;
			pcd8544_clear();
			sprintf(buffer, "X: %d Y: %d", x, y);
			pcd8544_print_at(buffer, 0, 0);
		}
		// ���� ������ 3 (�����) ������
		else if (!(btns & (1 << 2)) && x > 0) {
			x--;
			pcd8544_clear();
			sprintf(buffer, "X: %d Y: %d", x, y);
			pcd8544_print_at(buffer, 0, 0);
		}
		// ���� ������ 4 (������) ������
		else if (!(btns & (1 << 3)) && x < 11) {
			x++;
			pcd8544_clear();
			sprintf(buffer, "X: %d Y: %d", x, y);
			pcd8544_print_at(buffer, 0, 0);
		}

		// ����������� �����
		pcd8544_set_cursor(x, y);
		pcd8544_send_char('*');

		_delay_ms(200);  // ������ �� �������� ������
	}

	return 0;
}
