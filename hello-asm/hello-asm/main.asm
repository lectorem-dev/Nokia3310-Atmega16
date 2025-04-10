.device atmega16        ; Определяем целевое устройство

; ----	Регистры
.def R_TEMP = r16               ; Регистр для отправки данных по SPI
.def R_DELAY_SHORT_L  = r17		; Счётчик задержки (младший байт)
.def R_DELAY_SHORT_H  = r18     ; Счётчик задержки (старший байт)

; ---- Код
.cseg
.org 0 JMP main		; Точка входа в программу

; ---- Инициализация стека
STACK_INIT:
    ldi R_TEMP, high(RAMEND)
    out SPH, R_TEMP
    ldi R_TEMP, low(RAMEND)
    out SPL, R_TEMP
    ret

; ---- Функция задержки
DELAY_SHORT:
    ldi R_DELAY_SHORT_L , 255	; Задаем значения для задержки
    ldi R_DELAY_SHORT_H , 25
DELAY_LOOP:
    dec R_DELAY_SHORT_L         ; Уменьшаем R_DELAY_SHORT_L 
    brne DELAY_LOOP             ; Если R_DELAY_SHORT_L  не ноль, продолжаем цикл
    dec R_DELAY_SHORT_H         ; Уменьшаем R_DELAY_SHORT_H 
    brne DELAY_LOOP             ; Если R_DELAY_SHORT_H  не ноль, продолжаем цикл
    ret

; ---- Инициализация SPI
SPI_INIT:
    ldi R_TEMP, 0b10110000      ; SS, SCK, MOSI на выход
    out DDRB, R_TEMP            ; Отправляем в конфигуратор порта
    sbi PORTB, 4                ; SS = 1
    sbi PORTB, 6                ; MOSI = 1
    ldi R_TEMP, 0b11010001      ; SPI, SPIE и SPE = 1
    out SPCR, R_TEMP            ; MSTR = 1, SPR = 01, включаем SPI
    sei                         ; Разрешаем прерывания    
    ret

; ---- Отправка данных через SPI
SPI_TRANSMIT:
    out SPDR, R_TEMP            ; Передаем данные в регистр SPDR
    rcall DELAY_SHORT           ; Вызов задержки
    ret

; ---- Запись команды на LCD
LCD_WRITE_COMMAND:
    cbi PORTD, 7                ; D/C = 0 для отправки команды
    cbi PORTB, 4                ; SS = 0
    rcall SPI_TRANSMIT          ; Отправка команды
    sbi PORTB, 4                ; SS = 1
    ret

; ---- Запись данных на LCD
LCD_WRITE_DATA:
    sbi PORTD, 7                ; D/C = 1 для отправки данных
    cbi PORTB, 4                ; SS = 0
    rcall SPI_TRANSMIT          ; Отправка данных
    sbi PORTB, 4                ; SS = 1
    ret

; ---- Инициализация дисплея PCD8544
LCD_INIT:
    in r24, 0x11	; Чтение состояния порта
    ori r24, 0xC0
    out 0x11, r24	; Инициализация дисплея
    ldi r24, 0xBF
    out 0x12, r24
    sbi 0x12, 6		; Управление сбросом дисплея

    ldi R_TEMP, 0x21
    call LCD_WRITE_COMMAND
    ldi R_TEMP, 0xBA
    call LCD_WRITE_COMMAND
    ldi R_TEMP, 0x04
    call LCD_WRITE_COMMAND
    ldi R_TEMP, 0x14
    call LCD_WRITE_COMMAND
    ldi R_TEMP, 0x20
    call LCD_WRITE_COMMAND
    ldi R_TEMP, 0x0C
    call LCD_WRITE_COMMAND
    ret

; ---- Основная программа
MAIN:
    call SPI_INIT	; Инициализация SPI
    call LCD_INIT	; Инициализация дисплея

    call PRINT_K
    call PRINT_U
    call PRINT_K
    call PRINT_I
    call PRINT_N

while:
    RJMP while		; Бесконечный цикл


; ---- Метки для печати символов
PRINT_SPACE: 
    ldi R_TEMP, 0x00
    rcall LCD_WRITE_DATA
    ret

PRINT_K:
    ldi R_TEMP, 0x7f
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x08
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x14
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x22
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x41
    rcall LCD_WRITE_DATA
    call PRINT_SPACE
    ret

PRINT_U:
    ldi R_TEMP, 0x3f
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x40
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x40
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x40
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x3f
    rcall LCD_WRITE_DATA
    call PRINT_SPACE
    ret

PRINT_I:
    ldi R_TEMP, 0x00
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x41
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x7f
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x41
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x00
    rcall LCD_WRITE_DATA
    call PRINT_SPACE
    ret

PRINT_N:
    ldi R_TEMP, 0x7f
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x04
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x08
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x10
    rcall LCD_WRITE_DATA
    ldi R_TEMP, 0x7f
    rcall LCD_WRITE_DATA
    call PRINT_SPACE
    ret