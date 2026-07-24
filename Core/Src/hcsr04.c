#include "hcsr04.h"
#include "tim.h"

/* Rising edge 캡처 시점의 TIM3 카운터 값 */
volatile uint32_t ic_val1 = 0, ic_val2 = 0;
/* Rising/Falling edge 캡처 상태 플래그 (0: 첫 에지 대기, 1: 첫 에지 캡처 완료) */
volatile uint8_t is_first_captured = 0;
/* Echo 펄스의 폭(타이머 카운트 차이) */
volatile uint32_t pulse_width = 0;
/* Echo 펄스 폭을 cm로 환산한 거리값 */
volatile float distance_cm = 0.0f;
/* 20cm 이내 장애물 감지 여부 (1: 감지, 0: 비감지) */
volatile uint8_t obstacle_detected = 0;

/* HC-SR04 센서에 초음파 발신용 트리거 펄스를 생성한다. */
void Trig_Pulse(void)
{
    /* TRIG 핀을 HIGH로 올려 센서에 측정 시작 신호를 보낸다. */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    /* 약 10us 정도 유지한 뒤 LOW로 내려 트리거 펄스를 종료한다. */
    /* NOTE: 현재는 단순 for 루프로 지연을 구현하므로, 정확한 us 지연이 필요하면 HAL_Delay 대신 타이머 기반 지연으로 교체하는 것이 좋다. */
    for (volatile int i = 0; i < 100; i++);

    /* 트리거 신호 종료 */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

/* Input Capture 콜백 - TIM3 Channel1 (Echo 핀) */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        if (is_first_captured == 0) {
            /* Rising edge 캡처 */
            ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            is_first_captured = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        } else {
            /* Falling edge 캡처 */
            ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

            if (ic_val2 > ic_val1) {
                pulse_width = ic_val2 - ic_val1;
            } else {
                pulse_width = (0xFFFF - ic_val1) + ic_val2;
            }

            distance_cm = (float)pulse_width * 1.715f;

            /* 장애물 판단 (20cm 이내면 obstacle_detected = 1) */
            if (distance_cm <= 20.0f) {
                obstacle_detected = 1;
            } else {
                obstacle_detected = 0;
            }

            is_first_captured = 0;
        }
    }
}