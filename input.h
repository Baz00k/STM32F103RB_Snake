/*
 * input.h
 *
 *  Created on: Jun 1, 2022
 *      Author: Piotr
 */

#ifndef INPUT_H_
#define INPUT_H_

extern int input_threshold;
extern int last_input;

int get_input(int input_left, int input_right);
void calibrate_input(void);

#endif /* INPUT_H_ */
