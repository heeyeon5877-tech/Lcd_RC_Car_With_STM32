#ifndef LDR_LED_H
#define LDR_LED_H

#include "adc.h"
#include "tim.h"

extern uint32_t average_adc_value;

void Ldr_Led_Update(void);

#endif /* LDR_LED_H */