#include "adxl345.h"

static HAL_StatusTypeDef ADXL345_I2C_Reset(void)
{
    HAL_StatusTypeDef status = HAL_I2C_DeInit(&hi2c1);
    if (status != HAL_OK)
    {
        return status;
    }

    MX_I2C1_Init();
    return HAL_OK;
}

/* ADXL345 센서를 활성화하고 측정 범위/해상도를 설정한다. */
/*풀 레졸루션: 센서가 가질 수 있는 최고의 해상도를 항상 유지*/
// 측정 범위가 ±2g로 설정되어 있으므로, 출력 값은 ±2g 범위 내에서 10-bit 해상도로 제공된다.
/*측정범위가 변해도 출력 데이터의 해상도가 10bit로 유지되며, 
풀 레졸루션 모드에서는 출력 값이 실제 가속도 값에 비례하도록 자동으로 스케일링된다. */
void ADXL345_Init(void)
{
    uint8_t data;
    HAL_StatusTypeDef res_status = HAL_OK;
    /* DATA_FORMAT(0x31): ±2g, 풀 레졸루션 모드 설정 */
    data = 0x08;
    res_status = HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, ADXL345_REG_DATA_FORMAT, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* OFSZ(0x20): Z축 오프셋 보정 (+0.11g 보정을 위해 7 입력, 15.6mg/LSB 기준) */
    data = 7;  
    res_status = HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, ADXL345_REG_OFSZ, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* POWER_CTL(0x2D): Measure bit(D3) = 1 => 측정 모드 활성화 */
    data = 0x08;
    res_status = HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, ADXL345_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

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
     * X/Y/Z raw 값을 10비트 해상도 기준으로 대략 -60~60 범위로 변환
     * ADXL345 풀레졸루션 ±2g 출력을 약 -512~+511 범위로 받으므로,
     * 8.5로 나누면 -60~60 범위로 매핑된다.
     */
    // *ax = (float)rawX / 8.5f;
    // *ay = (float)rawY / 8.5f;
    // *az = (float)rawZ / 8.5f;
    *ax = (float)rawX;
    *ay = (float)rawY;
    *az = (float)rawZ;

    return HAL_OK;
}