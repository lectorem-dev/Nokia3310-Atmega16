.device atmega16        ; ���������� ������� ����������

; ----	��������
.def R_TEMP = r16               ; ������� ��� �������� ������ �� SPI
.def R_DELAY_SHORT_L  = r17		; ������� �������� (������� ����)
.def R_DELAY_SHORT_H  = r18     ; ������� �������� (������� ����)

; ---- ���
.cseg
.org 0 JMP main		; ����� ����� � ���������

; ---- ������������� �����
STACK_INIT:
    ldi R_TEMP, high(RAMEND)
    out SPH, R_TEMP
    ldi R_TEMP, low(RAMEND)
    out SPL, R_TEMP
    ret

; ---- ������� ��������
DELAY_SHORT:
    ldi R_DELAY_SHORT_L , 255	; ������ �������� ��� ��������
    ldi R_DELAY_SHORT_H , 25
DELAY_LOOP:
    dec R_DELAY_SHORT_L         ; ��������� R_DELAY_SHORT_L 
    brne DELAY_LOOP             ; ���� R_DELAY_SHORT_L  �� ����, ���������� ����
    dec R_DELAY_SHORT_H         ; ��������� R_DELAY_SHORT_H 
    brne DELAY_LOOP             ; ���� R_DELAY_SHORT_H  �� ����, ���������� ����
    ret

; ---- ������������� SPI
SPI_INIT:
    ldi R_TEMP, 0b10110000      ; SS, SCK, MOSI �� �����
    out DDRB, R_TEMP            ; ���������� � ������������ �����
    sbi PORTB, 4                ; SS = 1
    sbi PORTB, 6                ; MOSI = 1
    ldi R_TEMP, 0b11010001      ; SPI, SPIE � SPE = 1
    out SPCR, R_TEMP            ; MSTR = 1, SPR = 01, �������� SPI
    sei                         ; ��������� ����������    
    ret

; ---- �������� ������ ����� SPI
SPI_TRANSMIT:
    out SPDR, R_TEMP            ; �������� ������ � ������� SPDR
    rcall DELAY_SHORT           ; ����� ��������
    ret

; ---- ������ ������� �� LCD
LCD_WRITE_COMMAND:
    cbi PORTD, 7                ; D/C = 0 ��� �������� �������
    cbi PORTB, 4                ; SS = 0
    rcall SPI_TRANSMIT          ; �������� �������
    sbi PORTB, 4                ; SS = 1
    ret

; ---- ������ ������ �� LCD
LCD_WRITE_DATA:
    sbi PORTD, 7                ; D/C = 1 ��� �������� ������
    cbi PORTB, 4                ; SS = 0
    rcall SPI_TRANSMIT          ; �������� ������
    sbi PORTB, 4                ; SS = 1
    ret

; ---- ������������� ������� PCD8544
LCD_INIT:
    in r24, 0x11	; ������ ��������� �����
    ori r24, 0xC0
    out 0x11, r24	; ������������� �������
    ldi r24, 0xBF
    out 0x12, r24
    sbi 0x12, 6		; ���������� ������� �������

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

; ---- �������� ���������
MAIN:
    call SPI_INIT	; ������������� SPI
    call LCD_INIT	; ������������� �������

    call PRINT_K
    call PRINT_U
    call PRINT_K
    call PRINT_I
    call PRINT_N

while:
    RJMP while		; ����������� ����


; ---- ����� ��� ������ ��������
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