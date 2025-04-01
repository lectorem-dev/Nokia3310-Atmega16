#define F_CPU 16000000

#include "pcd8544.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	pcd8544_init();
	pcd8544_clear();
	pcd8544_print_at("Hello World", 1, 0);
	
	while (1) {
		_delay_ms(1000);
	}
	return 0;
}

