#include "game.h"

int obstacles[OBSTACLE_COUNT][2] = {
	{3, 4}, {5, 2}, {6, 4}, {8, 3}, {9, 2}
};

uint8_t game_lost = 0;

void draw_frame(void) {
	for (uint8_t x = 0; x < 12; x++) {
		pcd8544_set_cursor(x, 1);
		pcd8544_send_char('#');
	}
	for (uint8_t x = 0; x < 12; x++) {
		pcd8544_set_cursor(x, 5);
		pcd8544_send_char('#');
	}
	for (uint8_t y = 1; y < 5; y++) {
		pcd8544_set_cursor(0, y);
		pcd8544_send_char('#');
		pcd8544_set_cursor(11, y);
		pcd8544_send_char('#');
	}
}

void draw_obstacles(void) {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		uint8_t ox = obstacles[i][0];
		uint8_t oy = obstacles[i][1];
		pcd8544_set_cursor(ox, oy);
		pcd8544_send_char('@');
	}
}

int is_obstacle(int x, int y) {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i][0] == x && obstacles[i][1] == y) {
			return 1;
		}
	}
	return 0;
}

void display_coordinates(uint8_t x, uint8_t y) {
	char buffer[16];
	sprintf(buffer, "X: %02d  Y: %02d", x, y);
	pcd8544_print_at(buffer, 0, 0);
}

void display_win_message(void) {
	pcd8544_print_at("You Win!", 4, 2);
}

void display_lose_message(void) {
	pcd8544_print_at("Game Over!", 2, 2);
}

void display_timer(uint8_t time_left) {
	char buffer[16];
	sprintf(buffer, "Next in: %d", time_left);
	pcd8544_print_at(buffer, 0, 4);
}

void reset_game(uint8_t *x, uint8_t *y, uint8_t *game_won) {
	*x = 1;
	*y = 4;
	pcd8544_clear();
	draw_frame();
	draw_obstacles();
	display_coordinates(*x, *y);

	uint8_t target_x = 10, target_y = 2;
	pcd8544_set_cursor(target_x, target_y);
	pcd8544_send_char('$');

	pcd8544_set_cursor(*x, *y);
	pcd8544_send_char('*');

	*game_won = 0;
	game_lost = 0;
}
