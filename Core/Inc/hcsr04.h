#ifndef HCSR04_H
#define HCSR04_H

#include "main.h"

/* HC-SR04의 TRIG 핀에 10us 정도의 펄스를 발생시키는 함수 */
void Trig_Pulse(void);

/* HC-SR04 Echo 캡처 관련 전역 변수 선언 */
extern volatile uint32_t ic_val1;
extern volatile uint32_t ic_val2;
extern volatile uint8_t is_first_captured;
extern volatile uint32_t pulse_width;
extern volatile float distance_cm;
extern volatile uint8_t obstacle_detected;

#endif /* HCSR04_H */