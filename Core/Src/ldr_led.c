#include "ldr_led.h"
#include "main.h"

extern TIM_HandleTypeDef htim4;

uint32_t average_adc_value = 0;

void Ldr_Led_Update(void) {
    uint32_t sum = 0;
    
    // ADC ���� �б� �����մϴ�.
    HAL_ADC_Start(&hadc1);

    for(int i =0; i<5; i++)
    {
        HAL_ADC_PollForConversion(&hadc1, 10);
        sum += HAL_ADC_GetValue(&hadc1);
    }
    // // ADC ��ȯ�� �Ϸ�� ������ ��ٸ��ϴ�.
    // HAL_ADC_PollForConversion(&hadc1, 10);  
    // uint32_t adc_value = HAL_ADC_GetValue(&hadc1);

    // // �� ��° ADC ���� �н��ϴ�.
    // HAL_ADC_PollForConversion(&hadc1, 10);
    // uint32_t adc_value2 = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    //ADC ���� ����� ����մϴ�.
    average_adc_value = sum / 5;

    uint32_t inverted = 4095 - average_adc_value; // LDR ���� ���� ������ ���� ����մϴ�.
    // Counter Period�� 999�� ������. pwm������ �����ϸ�
    uint32_t pwm_value = (inverted * 999) / 4095;

    // LED ��⸦ �����մϴ�.
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_value);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_value);
}