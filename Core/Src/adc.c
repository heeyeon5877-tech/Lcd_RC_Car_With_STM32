#include "adc.h"

ADC_HandleTypeDef hadc1;

int HAL_ADC_Init(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    return 0;
}

int HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig)
{
    (void)hadc;
    (void)sConfig;
    return 0;
}

int HAL_ADC_Start(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    return 0;
}

int HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout)
{
    (void)hadc;
    (void)Timeout;
    return 0;
}

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    return 2048u;
}

int HAL_ADC_Stop(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    return 0;
}

void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    if (HAL_ADC_Init(&hadc1) != 0) {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != 0) {
        Error_Handler();
    }
}
