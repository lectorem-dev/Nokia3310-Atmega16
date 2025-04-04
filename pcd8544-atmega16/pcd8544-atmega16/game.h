#ifndef GAME_H
#define GAME_H

#include "nokia-lcd.h"
#include <stdio.h>

#define OBSTACLE_COUNT 5

extern int obstacles[OBSTACLE_COUNT][2];
extern uint8_t game_lost;

void draw_frame(void);
void draw_obstacles(void);
int is_obstacle(int x, int y);
void display_coordinates(uint8_t x, uint8_t y);
void display_win_message(void);
void display_lose_message(void);
void display_timer(uint8_t time_left);
void reset_game(uint8_t *x, uint8_t *y, uint8_t *game_won);

#endif