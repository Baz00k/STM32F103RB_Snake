/*
 * input.c
 *
 *  Created on: Jun 1, 2022
 *      Author: Piotr
 */

#include "input.h"
#include "adc.h"

int input_threshold = 2500;

int last_input = 0;
int last_input_left = 0;
int last_input_right = 0;

int get_input(int input_left, int input_right) {
	// -1 - input left
	// 0 - no input
	// 1 - input right

	// simulate button click with light sensors (generate input only once)
	if (input_left < input_threshold && last_input != -1
			&& last_input_left > input_left) {
		last_input = -1;
	} else if (input_right < input_threshold && last_input != 1
			&& last_input_right > input_right) {
		last_input = 1;
	} else {
		last_input = 0;
	}

	last_input_left = input_left;
	last_input_right = input_right;

	return last_input;
}

void calibrate_input() {
	int mid = (adc_value[1] + adc_value[2]) / 2;
	input_threshold = mid * 0.9;
}
