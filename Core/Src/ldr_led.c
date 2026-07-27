#include "ldr_led.h"

uint32_t average_adc_value = 0;

void Ldr_Led_Update(void) {
    // ADC 값을 읽기 시작합니다.
    HAL_ADC_Start(&hadc1);

    // ADC 변환이 완료될 때까지 기다립니다.
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);

    // 두 번째 ADC 값을 읽습니다.
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t adc_value2 = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    // 두 ADC 값의 평균을 계산합니다.
    average_adc_value = (adc_value + adc_value2) / 2;

    uint32_t inverted = 4095 - average_adc_value; // LDR 값에 따라 반전된 값을 계산합니다.
    // Counter Period를 999로 설정함. pwm값으로 스케일링
    uint32_t pwm_value = (inverted * 999) / 4095;

    // LED 밝기를 조절합니다.
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_value);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_value);
}