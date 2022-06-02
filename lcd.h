/*
 * lcd.h
 *
 *  Created on: May 30, 2022
 *      Author: Piotr
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include "stm32f1xx.h"

#define LCD_WIDTH				84
#define LCD_HEIGHT				48

#define SCALE					3
#define SCALED_WIDTH			(LCD_WIDTH / SCALE)
#define SCALED_HEIGHT			(LCD_HEIGHT / SCALE)

#define LCD_DC			GPIO_PIN_1
#define LCD_CE			GPIO_PIN_2
#define LCD_RST			GPIO_PIN_3

extern SPI_HandleTypeDef spi;
extern TIM_HandleTypeDef tim4;
extern TIM_OC_InitTypeDef oc;

void lcd_setup(void);

void lcd_clear(void);
void lcd_draw_bitmap(const uint8_t* data);
void lcd_draw_pixel(int x, int y);
void lcd_draw_text(int row, int col, const char* text);
void lcd_draw_line(int x1, int y1, int x2, int y2);
void lcd_setup_backlight(void);
void lcd_set_backlight(float val);

void lcd_copy(void);

#endif /* LCD_H_ */
