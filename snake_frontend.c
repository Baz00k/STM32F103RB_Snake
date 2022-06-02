/*
 * snake_frontend.c
 *
 *  Created on: Jun 1, 2022
 *      Author: Piotr
 */

#include <string.h>
#include <stdlib.h>
#include "snake_backend.h"
#include "snake_frontend.h"
#include "lcd.h"
#include "input.h"

void display_points(PointList* points) {
	while (points) {
		lcd_draw_pixel(points->x, points->y);
		points = points->next;
	}
}

void display_end_screen() {
	char buf[5];
	char result[20] = "Wynik: ";
	itoa(points, buf, 10);

	HAL_Delay(600);
	lcd_clear();
	lcd_draw_text(2, 16, "Game over");
	lcd_draw_text(3, 17, strcat(result, buf));
	lcd_copy();
	while (1) {
	}
}

enum Direction get_next_move(enum Direction previous, int input_left,
		int input_right) {
	int input = get_input(input_left, input_right);

	switch (input) {
	case -1:
		if (previous == UP)
			return LEFT;
		else if (previous == RIGHT)
			return UP;
		else if (previous == DOWN)
			return RIGHT;
		else if (previous == LEFT)
			return DOWN;
		break;
	case 1:
		if (previous == UP)
			return RIGHT;
		else if (previous == RIGHT)
			return DOWN;
		else if (previous == DOWN)
			return LEFT;
		else if (previous == LEFT)
			return UP;
		break;
	default:
		return previous;
	}

	return previous;
}
