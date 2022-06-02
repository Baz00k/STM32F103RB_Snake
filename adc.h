/*
 * adc.h
 *
 *  Created on: May 31, 2022
 *      Author: Piotr
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include "stm32f1xx.h"

#define ADC_CHANNELS	3
#define ADC_POT			GPIO_PIN_0
#define ADC_LEFT		GPIO_PIN_1
#define ADC_RIGHT		GPIO_PIN_2

extern DMA_HandleTypeDef dma;
extern ADC_HandleTypeDef adc;

extern uint16_t adc_value[ADC_CHANNELS];

void adc_setup(void);

#endif /* ADC_H_ */
