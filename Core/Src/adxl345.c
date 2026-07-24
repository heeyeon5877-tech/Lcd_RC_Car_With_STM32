#include "adxl345.h"

/* ADXL345 센서를 활성화하고 측정 범위/해상도를 설정한다. */
/*풀 레졸루션: 센서가 가질 수 있는 최고의 해상도를 항상 유지*/
// 측정 범위가 ±2g로 설정되어 있으므로, 출력 값은 ±2g 범위 내에서 10-bit 해상도로 제공된다.
/*측정범위가 변해도 출력 데이터의 해상도는 10bit로 유지되며, 
풀 레졸루션 모드에서는 출력 값이 실제 가속도 값에 비례하도록 자동으로 스케일링된다. */
void ADXL345_Init(void)
{
    uint8_t data;

    /* DATA_FORMAT(0x31): ±2g, 풀 레졸루션 모드 설정 */
    data = 0x08;
    HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, ADXL345_REG_DATA_FORMAT, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* OFSZ(0x20): Z축 오프셋 보정 (+0.11g 보정을 위해 7 입력, 15.6mg/LSB 기준) */
    data = 7; 
    HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, ADXL345_REG_OFSZ, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* POWER_CTL(0x2D): Measure bit(D3) = 1 => 측정 모드 활성화 */
    data = 0x08;
    HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, ADXL345_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

}

/* ADXL345의 6바이트 가속도 값을 읽어 X/Y/Z 값을 g 단위로 변환한다. */
HAL_StatusTypeDef ADXL345_Read(float *ax, float *ay, float *az)
{
    uint8_t raw[6];
    int16_t rawX, rawY, rawZ;

    /* DATAX0(0x32)부터 6바이트를 읽어 X/Y/Z의 하위/상위 바이트를 획득 */
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, ADXL345_ADDR, ADXL345_REG_DATAX0,
                                                 I2C_MEMADD_SIZE_8BIT, raw, 6, 100);
    if (status != HAL_OK)
    {
        return status;
    }

    /* Little Endian 형식으로 저장된 2바이트 데이터를 signed 16-bit 값으로 변환 */
    rawX = (int16_t)(raw[1] << 8 | raw[0]);
    rawY = (int16_t)(raw[3] << 8 | raw[2]);
    rawZ = (int16_t)(raw[5] << 8 | raw[4]);

    /*
     * X축 방향의 가속도/기울기 값(좌우 틸트에 해당)
     * Y축 방향의 가속도/기울기 값(전후 틸트에 해당)
     * Z축 방향의 가속도/기울기 값(수직 축 기준 편차에 해당)
     */
    /* 풀 레졸루션 ±2g 기준 scale factor: 3.9mg/LSB */
    *ax = rawX * 0.0039f;
    *ay = rawY * 0.0039f;
    *az = rawZ * 0.0039f;

    return HAL_OK;
}