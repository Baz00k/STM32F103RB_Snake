/*
 * snake_backend.c
 *
 *  Created on: Jun 1, 2022
 *      Author: Piotr
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "snake_backend.h"
#include "lcd.h"

int points = 0;

/*
 * Removes from the list or returns false
 */
bool remove_from_list(PointList* elt, PointList** list) {
	PointList *currP, *prevP;
	prevP = NULL;

	for (currP = *list; currP != NULL; prevP = currP, currP = currP->next) {
		if (is_same_place(currP, elt)) {
			if (prevP == NULL) {
				*list = currP->next;
			} else {
				prevP->next = currP->next;
			}
			free(currP);
			return true;
		}
	}
	return false;
}

enum Status move_snake(Board* board, enum Direction dir) {
	// Create a new beginning. Check boundaries.
	PointList* beginning = next_move(board, dir);
	if (beginning == NULL) {
		return FAILURE;
	}

	// If we've gone backwards, don't do anything
	if (board->snake->next && is_same_place(beginning, board->snake->next)) {
		beginning->next = NULL;
		free(beginning);
		return SUCC;
	}

	// Check for collisions
	if (list_contains(beginning, board->snake)) {
		return FAILURE;
	}

	// Check for food
	if (list_contains(beginning, board->foods)) {
		// Attach the beginning to the rest of the snake;
		beginning->next = board->snake;
		board->snake = beginning;
		remove_from_list(beginning, &(board->foods));
		add_new_food(board);
		points++;

		return SUCC;
	}

	// Attach the beginning to the rest of the snake
	beginning->next = board->snake;
	board->snake = beginning;

	// Cut off the end
	PointList* end = board->snake;
	while (end->next->next) {
		end = end->next;
	}
	free(end->next);
	end->next = NULL;

	return SUCC;
}

bool is_same_place(PointList* cell1, PointList* cell2) {
	return cell1->x == cell2->x && cell1->y == cell2->y;
}

PointList* next_move(Board* board, enum Direction dir) {
	PointList* snake = board->snake;
	int new_x = snake->x;
	int new_y = snake->y;
	switch (dir) {
	case UP:
		new_y = snake->y - 1;
		break;
	case DOWN:
		new_y = snake->y + 1;
		break;
	case LEFT:
		new_x = snake->x - 1;
		break;
	case RIGHT:
		new_x = snake->x + 1;
		break;
	}

	/* Wrap snake around the screen */
	if (new_x < 0)
		new_x = board->xmax + new_x;
	else if (new_y < 0)
		new_y = board->ymax + new_y;
	else if (new_x >= board->xmax)
		new_x = new_x - board->xmax;
	else if (new_y >= board->ymax)
		new_y = new_y - board->ymax;
	return create_cell(new_x, new_y);

}

PointList* create_random_cell(int xmax, int ymax) {
	return create_cell(rand() % xmax, rand() % ymax);
}

void add_new_food(Board* board) {
	PointList* new_food;
	do {
		new_food = create_random_cell(board->xmax, board->ymax);
	} while (list_contains(new_food, board->foods)
			|| list_contains(new_food, board->snake));
	new_food->next = board->foods;
	board->foods = new_food;
}

bool list_contains(PointList* cell, PointList* list) {
	PointList* s = list;
	while (s) {
		if (is_same_place(s, cell)) {
			return true;
		}
		s = s->next;
	}
	return false;
}

PointList* create_cell(int x, int y) {
	PointList* cell = malloc(sizeof(*cell));
	cell->x = x;
	cell->y = y;
	cell->next = NULL;
	return cell;
}

Board* create_board(PointList* snake, PointList* foods, int xmax, int ymax) {
	Board* board = malloc(sizeof(*board));
	board->foods = foods;
	board->snake = snake;
	board->xmax = xmax;
	board->ymax = ymax;
	return board;
}

PointList* create_snake() {
	int halfY = SCALED_HEIGHT / 2;
	int halfX = SCALED_WIDTH / 2;

	PointList* a = create_cell(halfX, halfY);
	PointList* b = a;

	for(int i = 1; i < SNAKE_START_LENGTH; ++i) {
		b->next = create_cell(halfX, ++halfY);
		b = b->next;
	}

	return a;
}
