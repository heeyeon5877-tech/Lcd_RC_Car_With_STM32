#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

typedef struct
{
    uint32_t dummy;
} ADC_HandleTypeDef;

typedef struct
{
    uint32_t Channel;
    uint32_t Rank;
    uint32_t SamplingTime;
} ADC_ChannelConfTypeDef;

#define ADC_CHANNEL_0 0u
#define ADC_SAMPLETIME_3CYCLES 3u

extern ADC_HandleTypeDef hadc1;

void MX_ADC1_Init(void);

int HAL_ADC_Init(ADC_HandleTypeDef *hadc);
int HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig);
int HAL_ADC_Start(ADC_HandleTypeDef *hadc);
int HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout);
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);
int HAL_ADC_Stop(ADC_HandleTypeDef *hadc);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
