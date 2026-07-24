#ifndef ADXL345_H
#define ADXL345_H

#include "i2c.h"

/* ADXL345 I2C slave address (SDO = GND 기준) */
#define ADXL345_ADDR            (0x53 << 1)

/* ADXL345 내부 레지스터 정의 */
#define ADXL345_REG_DEVID       0x00  /* Device ID 레지스터 */

//하드웨어 오프셋 보정을 위한 레지스터 추가
#define ADXL345_REG_OFSZ        0x20  /* Z축 오프셋 보정 레지스터 */

#define ADXL345_REG_POWER_CTL   0x2D  /* 측정 모드 제어 레지스터 */
#define ADXL345_REG_DATA_FORMAT 0x31  /* 출력 해상도/측정 범위 설정 레지스터 */
#define ADXL345_REG_DATAX0      0x32  /* X 축 하위 바이트 시작 주소 */

#define ADXL345_SCALE_FACTOR_FULLRES  0.0039f

/* 센서 초기화: 측정 모드 활성화 및 ±2g, 풀 레졸루션 설정 */
void ADXL345_Init(void);

/* 센서 값 읽기: X/Y/Z 축 값을 g 단위로 반환 */
HAL_StatusTypeDef ADXL345_Read(float *ax, float *ay, float *az);

#endif /* ADXL345_H */