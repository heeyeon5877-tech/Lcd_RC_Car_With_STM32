/*
 * STM32F411RE HAL 기반 ILI9341 SPI 드라이버
 * - SPI1 + DMA (픽셀 데이터 대량 전송)
 * - CS / DC / RST GPIO 소프트웨어 제어
 * - ID 레지스터 읽기로 초기화 검증 (MISO 연결 시)
 */

#ifndef ILI9341_H
#define ILI9341_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* 사용자 설정 */
#define LCD_SPI_HANDLE      hspi1

#define LCD_CS_PORT         LCD_CS_GPIO_Port
#define LCD_CS_PIN          LCD_CS_Pin
#define LCD_DC_PORT         LCD_DC_GPIO_Port
#define LCD_DC_PIN          LCD_DC_Pin
#define LCD_RST_PORT        LCD_RST_GPIO_Port
#define LCD_RST_PIN         LCD_RST_Pin

/* 해상도 */
#define ILI9341_WIDTH       320
#define ILI9341_HEIGHT      240
#define FACE_WIDTH          160
#define FACE_HEIGHT         160

/* 주요 색상 (RGB565) */
#define ILI9341_BLACK       0x0000
#define ILI9341_WHITE       0xFFFF
#define ILI9341_RED         0xF800
#define ILI9341_GREEN       0x07E0
#define ILI9341_BLUE        0x001F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_CYAN        0x07FF
#define ILI9341_MAGENTA     0xF81F
#define BG_COLOR            ILI9341_BLACK

static int16_t last_x = -1000, last_y = -1000;

/** LI9341 초기화 (Reset -> 초기화 시퀀스 -> Sleep Out -> Display On) */
void ILI9341_Init(SPI_HandleTypeDef *hspi);
bool ILI9341_VerifyID(uint8_t id_out[4]);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ILI9341_DrawBuffer(const uint16_t *buf, uint32_t pixel_count);
void ILI9341_DrawBufferBytes(const uint8_t *data, uint32_t len);
void ILI9341_SetRotation(uint8_t rotation);
void ILI9341_DrawImageFromArray(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void Face_UpdatePosition(int16_t new_x, int16_t new_y, const uint16_t* img);

#endif