#include "nokia-lcd.h"
#include "font.h"
#include "buttons.h"
#include "game.h"

#include <util/delay.h>
#include <avr/io.h>

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

		if (is_obstacle(next_x, next_y)) {
			game_lost = 1;
			display_updated = 0;
			continue;
			} else if (next_x != last_x || next_y != last_y) {
			pcd8544_set_cursor(last_x, last_y);
			if (last_x == 10 && last_y == 2) {
				pcd8544_send_char('$');
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
