#ifndef PCD8544_H
#define PCD8544_H

#include <avr/io.h>

// Определения пинов
#define PIN_RESET PB2
#define PIN_DC    PB3
#define PIN_SCE   PB4
#define PIN_SDIN  PB5
#define PIN_SCLK  PB7

// Команды для дисплея
#define LCD_C     0x00
#define LCD_D     0x01

#define LCD_X     84
#define LCD_Y     48

// Функции управления дисплеем
void pcd8544_init(void);
void pcd8544_send(uint8_t dc, uint8_t data);
void pcd8544_print_string(char *str);
void pcd8544_send_char(uint8_t ch);
void pcd8544_clear(void);
void pcd8544_set_cursor(uint8_t x, uint8_t y);
void pcd8544_print_at(char *str, uint8_t x, uint8_t y);

#endif
