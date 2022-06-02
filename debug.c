/*
 * debug.c
 *
 *  Created on: Jun 1, 2022
 *      Author: Piotr
 */

#include <stdlib.h>
#include "lcd.h"
#include "adc.h"

void print_debug_info() {
	char buf[4];

	itoa(adc_value[1], buf, 10);
	lcd_draw_text(0, 0, buf);
	itoa(adc_value[2], buf, 10);
	lcd_draw_text(1, 0, buf);
}
