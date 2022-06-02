/*
 * lcd.c
 *
 *  Created on: May 30, 2022
 *      Author: Piotr
 */

#include <string.h>
#include <math.h>
#include "lcd.h"
#include "font.h"

#define PCD8544_FUNCTION_SET		0x20
#define PCD8544_DISP_CONTROL		0x08
#define PCD8544_DISP_NORMAL			0x0c
#define PCD8544_SET_Y				0x40
#define PCD8544_SET_X				0x80
#define PCD8544_H_TC				0x04
#define PCD8544_H_BIAS				0x10
#define PCD8544_H_VOP				0x80

#define LCD_BUFFER_SIZE			(LCD_WIDTH * LCD_HEIGHT / 8)

//slope for calculating value for display brightness
#define INPUT_START		3
#define INPUT_END		4095
#define OUTPUT_START	0
#define OUTPUT_END		140

#define SLOPE			(1.0 * (OUTPUT_END - OUTPUT_START) / (INPUT_END - INPUT_START))

SPI_HandleTypeDef spi;
TIM_HandleTypeDef tim4;
TIM_OC_InitTypeDef oc;

static uint8_t lcd_buffer[LCD_BUFFER_SIZE];

void lcd_cmd(uint8_t cmd) {
	HAL_GPIO_WritePin(GPIOC, LCD_CE | LCD_DC, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&spi, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOC, LCD_CE | LCD_DC, GPIO_PIN_SET);
}

void lcd_setup(void) {
	HAL_GPIO_WritePin(GPIOC, LCD_RST, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, LCD_RST, GPIO_PIN_SET);

	lcd_cmd(PCD8544_FUNCTION_SET | 1);
	lcd_cmd(PCD8544_H_BIAS | 4);
	lcd_cmd(PCD8544_H_VOP | 0x3a);  //set contrast of the sreen
	lcd_cmd(PCD8544_FUNCTION_SET);
	lcd_cmd(PCD8544_DISP_NORMAL);
}

void lcd_clear(void) {
	memset(lcd_buffer, 0, LCD_BUFFER_SIZE);
}

void lcd_draw_bitmap(const uint8_t* data) {
	memcpy(lcd_buffer, data, LCD_BUFFER_SIZE);
}

inline void lcd_draw_pixel(int x, int y) {
	x *= SCALE;
	y *= SCALE;

	int tmpY = y;

	for (int i = 0; i < SCALE; ++i) {
		for (int j = 0; j < SCALE; ++j) {
			lcd_buffer[x + (y >> 3) * LCD_WIDTH] |= 1 << (y & 7);
			++y;
		}
		y = tmpY;
		++x;
	}
}

void lcd_draw_text(int row, int col, const char* text) {
	int i;
	uint8_t* pbuf = &lcd_buffer[row * LCD_WIDTH + col];
	while ((*text) && (pbuf < &lcd_buffer[LCD_BUFFER_SIZE - 6])) {
		int ch = *text++;
		const uint8_t* font = &font_ASCII[ch - ' '][0];
		for (i = 0; i < 5; i++)
			*pbuf++ = *font++;
		*pbuf++ = 0;
	}
}

void lcd_draw_line(int x1, int y1, int x2, int y2) {
	int dx, dy, sx, sy;
	if (x2 >= x1) {
		dx = x2 - x1;
		sx = 1;
	} else {
		dx = x1 - x2;
		sx = -1;
	}
	if (y2 >= y1) {
		dy = y1 - y2;
		sy = 1;
	} else {
		dy = y2 - y1;
		sy = -1;
	}

	int dx2 = dx << 1;
	int dy2 = dy << 1;
	int err = dx2 + dy2;
	while (1) {
		lcd_draw_pixel(x1, y1);
		if (err >= dy) {
			if (x1 == x2)
				break;
			err += dy2;
			x1 += sx;
		}
		if (err <= dx) {
			if (y1 == y2)
				break;
			err += dx2;
			y1 += sy;
		}
	}
}

void lcd_copy(void) {
	HAL_GPIO_WritePin(GPIOC, LCD_DC, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, LCD_CE, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&spi, lcd_buffer, LCD_BUFFER_SIZE, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOC, LCD_CE, GPIO_PIN_SET);
}

void lcd_setup_backlight(void) {
	tim4.Instance = TIM4;
	tim4.Init.Period = 100 - 1;
	tim4.Init.Prescaler = 800 - 1;
	tim4.Init.ClockDivision = 0;
	tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim4.Init.RepetitionCounter = 0;
	tim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_PWM_Init(&tim4);

	oc.OCMode = TIM_OCMODE_PWM1;
	oc.Pulse = 100;
	oc.OCPolarity = TIM_OCPOLARITY_HIGH;
	oc.OCNPolarity = TIM_OCNPOLARITY_LOW;
	oc.OCFastMode = TIM_OCFAST_ENABLE;
	oc.OCIdleState = TIM_OCIDLESTATE_SET;
	oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_1);
}

void lcd_set_backlight(float val) {
	int out_val = OUTPUT_START + SLOPE * (val - INPUT_START);

	__HAL_TIM_SET_COMPARE(&tim4, TIM_CHANNEL_1, out_val);
}
