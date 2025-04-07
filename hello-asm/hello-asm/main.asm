.include "m16def.inc"

.def temp = r16
.def data = r17
.def bitc = r18

.equ PIN_RESET = 2
.equ PIN_DC    = 3
.equ PIN_SCE   = 4
.equ PIN_SDIN  = 5
.equ PIN_SCLK  = 7

.cseg
.org 0x00
	rjmp main

; ========================
; Отправка байта в дисплей
; ========================
send:
	; DC бит
	sbrc data, 7
	sbi PORTB, PIN_SDIN
	sbrs data, 7
	cbi PORTB, PIN_SDIN
	ldi bitc, 8
send_loop:
	; Выставить бит данных
	sbrc data, 7
	sbi PORTB, PIN_SDIN
	sbrs data, 7
	cbi PORTB, PIN_SDIN

	; Пульс по SCLK
	sbi PORTB, PIN_SCLK
	cbi PORTB, PIN_SCLK

	; Сдвиг
	lsl data
	dec bitc
	brne send_loop
	ret

; ========================
; Главная программа
; ========================
main:
	; Настройка портов
	ldi temp, (1<<PIN_RESET)|(1<<PIN_DC)|(1<<PIN_SCE)|(1<<PIN_SDIN)|(1<<PIN_SCLK)
	out DDRB, temp

	; Сброс дисплея
	cbi PORTB, PIN_RESET
	rcall delay
	sbi PORTB, PIN_RESET

	; Инициализация дисплея
	; (посылаем команды)
	cbi PORTB, PIN_DC    ; командный режим
	cbi PORTB, PIN_SCE
	ldi data, 0x21       ; расширенный режим
	rcall send
	ldi data, 0xB1       ; контраст
	rcall send
	ldi data, 0x04       ; температурный коэффициент
	rcall send
	ldi data, 0x14       ; bias
	rcall send
	ldi data, 0x20       ; базовый режим
	rcall send
	ldi data, 0x0C       ; нормальный режим
	rcall send
	sbi PORTB, PIN_SCE

	ldi ZH, high(hello * 2)
	ldi ZL, low(hello * 2)

	ldi temp, 6 * 5      ; 6 буквы по 5 байт Hello_
print_loop:
	lpm data, Z+
	sbi PORTB, PIN_DC    ; данные
	cbi PORTB, PIN_SCE
	rcall send
	sbi PORTB, PIN_SCE
	dec temp
	brne print_loop

loop:
	rjmp loop

; ========================
; Задержка (грубая)
; ========================
delay:
	ldi temp, 100
delay_loop:
	dec temp
	brne delay_loop
	ret

; ========================
; Таблица символов
; ========================
hello:
	.db 0x7F, 0x08, 0x08, 0x08, 0x7F ; H
	.db 0x38, 0x54, 0x54, 0x54, 0x18 ; e
	.db 0x00, 0x41, 0x7F, 0x40, 0x00 ; l
	.db 0x00, 0x41, 0x7F, 0x40, 0x00 ; l
	.db 0x38, 0x44, 0x44, 0x44, 0x38 ; o
