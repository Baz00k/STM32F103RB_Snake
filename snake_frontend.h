/*
 * snake_frontend.h
 *
 *  Created on: Jun 1, 2022
 *      Author: Piotr
 */

#ifndef SNAKE_FRONTEND_H_
#define SNAKE_FRONTEND_H_

enum Direction get_next_move(enum Direction previous, int input_left,
		int input_right);
void display_points(PointList* points);
void display_end_screen(void);

#endif /* SNAKE_FRONTEND_H_ */
