#include "pcd8544.h"
#include "pcd8544_font.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#define SH_LD PD2
#define CLK   PD3
#define QH    PD4

uint8_t game_lost = 0;

#define OBSTACLE_COUNT 5
int obstacles[OBSTACLE_COUNT][2] = {
	{3, 4}, {5, 2}, {6, 4}, {8, 3}, {9, 2}
};

// Проверка, не пытается ли игрок вступить на препядствие
int is_obstacle(int x, int y) {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i][0] == x && obstacles[i][1] == y) {
			return 1;
		}
	}
	return 0;
}


// Инициализация сдвигового регистра
void shift_register_init() {
	DDRD |= (1 << SH_LD) | (1 << CLK);
	DDRD &= ~(1 << QH);
	PORTD |= (1 << SH_LD);
	PORTD &= ~(1 << CLK);
	_delay_us(5);
}

// Чтение состояния кнопок
uint8_t read_buttons() {
	uint8_t data = 0;
	PORTD &= ~(1 << SH_LD);   // Загрузка данных с кнопок
	_delay_us(5);
	PORTD |= (1 << SH_LD);    // Переключение в режим сдвига
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

// Отображение рамки
void draw_frame(void) {
	// Верхняя граница начинается с 1-й строки (чтобы не перекрывать координаты)
	for (uint8_t x = 0; x < 12; x++) {
		pcd8544_set_cursor(x, 1);
		pcd8544_send_char('#');
	}
	// Нижняя граница
	for (uint8_t x = 0; x < 12; x++) {
		pcd8544_set_cursor(x, 5);
		pcd8544_send_char('#');
	}
	// Левая и правая границы
	for (uint8_t y = 1; y < 5; y++) {
		pcd8544_set_cursor(0, y);
		pcd8544_send_char('#');
		pcd8544_set_cursor(11, y);
		pcd8544_send_char('#');
	}
}

// Отображение координат
void display_coordinates(uint8_t x, uint8_t y) {
	char buffer[16];
	// Форматируем строку как "X: 01 Y: 01" с двумя знаками для каждого числа
	sprintf(buffer, "X: %02d Y: %02d", x, y);
	pcd8544_print_at(buffer, 0, 0);
}

// Отображение сообщения о победе
void display_win_message(void) {
	pcd8544_print_at("You Win!", 4, 2);  // Сообщение о победе на экране
}

void display_lose_message(void) {
	pcd8544_print_at("Game Over!", 3, 2);
}

// Отображение таймера отсчета
void display_timer(uint8_t time_left) {
	char buffer[16];
	sprintf(buffer, "Next in: %d", time_left);
	pcd8544_print_at(buffer, 0, 4);  // Таймер будет внизу экрана
}

// Отрисовка препятствий
void draw_obstacles() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		uint8_t ox = obstacles[i][0];
		uint8_t oy = obstacles[i][1];
		pcd8544_set_cursor(ox, oy);
		pcd8544_send_char('@');  // Символ препятствия
	}
}

// Сброс игры и создание карты
void reset_game(uint8_t *x, uint8_t *y, uint8_t *game_won) {
	*x = 1;
	*y = 4;

	pcd8544_clear();
	draw_frame();
	draw_obstacles(); // ? добавлена отрисовка препятствий
	display_coordinates(*x, *y);

	uint8_t target_x = 10, target_y = 2;

	pcd8544_set_cursor(target_x, target_y);
	pcd8544_send_char('X');  // Целевая точка

	pcd8544_set_cursor(*x, *y);
	pcd8544_send_char('*');

	*game_won = 0;
	game_lost = 0;
}

int main(void) {
	pcd8544_init();
	pcd8544_clear();
	shift_register_init();

	uint8_t x = 1, y = 4;
	uint8_t last_x = x, last_y = y;
	uint8_t game_won = 0;
	uint8_t display_updated = 1;

	reset_game(&x, &y, &game_won);

	while (1) {
		if (game_lost) {
			display_lose_message();
			for (uint8_t i = 3; i > 0; i--) {
				display_timer(i);
				_delay_ms(1000);
			}
			reset_game(&x, &y, &game_won);
			last_x = x;
			last_y = y;
			display_updated = 1;
			continue;
		}
		
		if (x == 10 && y == 2 && !game_won) {
			game_won = 1;
			display_updated = 0;
		}

		if (game_won) {
			display_win_message();

			for (uint8_t i = 3; i > 0; i--) {
				display_timer(i);
				_delay_ms(1000);
			}

			reset_game(&x, &y, &game_won);
			display_updated = 1;
			continue;
		}

		uint8_t btns = read_buttons();
		uint8_t next_x = x;
		uint8_t next_y = y;

		if (!(btns & (1 << 0)) && y > 2) next_y--;
		else if (!(btns & (1 << 1)) && y < 4) next_y++;
		else if (!(btns & (1 << 2)) && x > 1) next_x--;
		else if (!(btns & (1 << 3)) && x < 10) next_x++;

		// Новая проверка: не наступаем на препятствие
		if (is_obstacle(next_x, next_y)) {
			game_lost = 1;
			display_updated = 0;
			continue;
			} else if (next_x != last_x || next_y != last_y) {

			pcd8544_set_cursor(last_x, last_y);
			if (last_x == 10 && last_y == 2) {
				pcd8544_send_char('X');
				} else if (is_obstacle(last_x, last_y)) {
				pcd8544_send_char('@');
				} else {
				pcd8544_send_char(' ');
			}

			x = next_x;
			y = next_y;

			pcd8544_set_cursor(x, y);
			pcd8544_send_char('*');

			display_coordinates(x, y);

			last_x = x;
			last_y = y;
		}

		_delay_ms(200);
	}

	return 0;
}
