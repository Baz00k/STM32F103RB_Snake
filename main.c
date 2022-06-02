#include <stdlib.h>
#include <stm32f103xb.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_rcc_ex.h>
#include <stm32f1xx_hal_spi.h>
#include <system_stm32f1xx.h>

#include "adc.h"
#include "lcd.h"
#include "snake_backend.h"
#include "snake_frontend.h"

int main(void) {

	SystemCoreClock = 8000000; // taktowanie 8Mhz
	HAL_Init();

	// W³¹czanie zegarów na portach
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_SPI1_CLK_ENABLE()
	;
	__HAL_RCC_TIM4_CLK_ENABLE()
	;
	__HAL_RCC_DMA1_CLK_ENABLE()
	;
	__HAL_RCC_ADC1_CLK_ENABLE()
	;

	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_5 | GPIO_PIN_7;		// SCK, MOSI
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_INPUT;
	gpio.Pin = GPIO_PIN_6;						// MISO
	gpio.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_ANALOG;
	gpio.Pin = ADC_POT | ADC_LEFT | ADC_RIGHT;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = LCD_DC | LCD_CE | LCD_RST;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &gpio);
	HAL_GPIO_WritePin(GPIOC, LCD_CE | LCD_RST, GPIO_PIN_SET);

	spi.Instance = SPI1;
	spi.Init.Mode = SPI_MODE_MASTER;
	spi.Init.NSS = SPI_NSS_SOFT;
	spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;	// 1MHz
	spi.Init.Direction = SPI_DIRECTION_2LINES;
	spi.Init.CLKPhase = SPI_PHASE_1EDGE;
	spi.Init.CLKPolarity = SPI_POLARITY_LOW;
	spi.Init.DataSize = SPI_DATASIZE_8BIT;
	spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi.Init.TIMode = SPI_TIMODE_DISABLE;
	spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&spi);

	__HAL_SPI_ENABLE(&spi);

	adc_setup();

	lcd_setup();
	lcd_setup_backlight();
	calibrate_input();

	// SNAKE!

	//get random values each time
	//we can't use time so we use value from light sensor
	srand(adc_value[1]);

	enum Direction dir = UP;

	Board* board = create_board(create_snake(), NULL, SCALED_WIDTH,
	SCALED_HEIGHT);
	int i;
	for (i = 0; i < 6; i++) {
		add_new_food(board);
	}

	while (true) {

		lcd_clear();
		display_points(board->snake);
		display_points(board->foods);
		lcd_copy();

		dir = get_next_move(dir, adc_value[2], adc_value[1]);
		enum Status status = move_snake(board, dir);
		if (status == FAILURE)
			break;

		lcd_set_backlight(adc_value[0]);

		HAL_Delay(100);
	}

	display_end_screen();

}
