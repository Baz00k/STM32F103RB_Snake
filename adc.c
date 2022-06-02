/*
 * adc.c
 *
 *  Created on: May 31, 2022
 *      Author: Piotr
 */

#include "adc.h"

DMA_HandleTypeDef dma;
ADC_HandleTypeDef adc;

uint16_t adc_value[ADC_CHANNELS];

void adc_setup() {
	RCC_PeriphCLKInitTypeDef adc_clk;
	adc_clk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	adc_clk.AdcClockSelection = RCC_ADCPCLK2_DIV2;
	HAL_RCCEx_PeriphCLKConfig(&adc_clk);

	adc.Instance = ADC1;
	adc.Init.ContinuousConvMode = ENABLE;
	adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	adc.Init.NbrOfConversion = ADC_CHANNELS;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.NbrOfDiscConversion = 1;
	HAL_ADC_Init(&adc);

	ADC_ChannelConfTypeDef adc_ch;
	adc_ch.Channel = ADC_CHANNEL_0;
	adc_ch.Rank = ADC_REGULAR_RANK_1;
	adc_ch.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	adc_ch.Channel = ADC_CHANNEL_1;
	adc_ch.Rank = ADC_REGULAR_RANK_2;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	adc_ch.Channel = ADC_CHANNEL_4;
	adc_ch.Rank = ADC_REGULAR_RANK_3;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	HAL_ADCEx_Calibration_Start(&adc);

	dma.Instance = DMA1_Channel1;
	dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	dma.Init.PeriphInc = DMA_PINC_DISABLE;
	dma.Init.MemInc = DMA_MINC_ENABLE;
	dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	dma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	dma.Init.Mode = DMA_CIRCULAR;
	dma.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&dma);
	__HAL_LINKDMA(&adc, DMA_Handle, dma);

	HAL_ADC_Start_DMA(&adc, (uint32_t*) adc_value, ADC_CHANNELS);
}
