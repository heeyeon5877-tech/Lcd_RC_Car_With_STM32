#include "ili9341.h"
#include "center_img.h"
 
/* ILI9341 커맨드 정의 */
#define CMD_SWRESET     0x01
#define CMD_RDDID       0x04
#define CMD_SLPOUT      0x11
#define CMD_DISPOFF     0x28
#define CMD_DISPON      0x29
#define CMD_CASET       0x2A
#define CMD_PASET       0x2B
#define CMD_RAMWR       0x2C
#define CMD_MADCTL      0x36
#define CMD_PIXFMT      0x3A
#define CMD_FRMCTR1     0xB1
#define CMD_DISCTRL     0xB6
#define CMD_PWCTR1      0xC0
#define CMD_PWCTR2      0xC1
#define CMD_VMCTR1      0xC5
#define CMD_VMCTR2      0xC7
#define CMD_GAMMASET    0x26
#define CMD_GMCTRP1     0xE0
#define CMD_GMCTRN1     0xE1
#define CMD_PWCTRA      0xCB
#define CMD_PWCTRB      0xCF
#define CMD_TIMCTRA     0xE8
#define CMD_TIMCTRB     0xEA
#define CMD_PWSEQCTRL   0xED
#define CMD_EN3GAM      0xF2
#define CMD_PUMPCTRL    0xF7
 
static SPI_HandleTypeDef *s_hspi;
static volatile bool s_dma_busy = false;
 
static inline void CS_Select(void)   { HAL_GPIO_WritePin(LCD_CS_PORT,  LCD_CS_PIN,  GPIO_PIN_RESET); }
static inline void CS_Deselect(void) { HAL_GPIO_WritePin(LCD_CS_PORT,  LCD_CS_PIN,  GPIO_PIN_SET);   }
static inline void DC_Command(void)  { HAL_GPIO_WritePin(LCD_DC_PORT,  LCD_DC_PIN,  GPIO_PIN_RESET); }
static inline void DC_Data(void)     { HAL_GPIO_WritePin(LCD_DC_PORT,  LCD_DC_PIN,  GPIO_PIN_SET);   }
 
static void WriteCommand(uint8_t cmd)
{
    CS_Select();
    DC_Command();
    HAL_SPI_Transmit(s_hspi, &cmd, 1, HAL_MAX_DELAY);
    CS_Deselect();
}
 
static void WriteData(const uint8_t *data, uint16_t len)
{
    CS_Select();
    DC_Data();
    HAL_SPI_Transmit(s_hspi, (uint8_t *)data, len, HAL_MAX_DELAY);
    CS_Deselect();
}
 
static void WriteData8(uint8_t val)
{
    WriteData(&val, 1);
}
 
// 대량 픽셀 데이터는 DMA로 (프레임버퍼 전송 등 성능이 중요한 구간)
static void WriteDataDMA(const uint8_t *data, uint32_t len)
{
    s_dma_busy = true;
    CS_Select();
    DC_Data();
    HAL_SPI_Transmit_DMA(s_hspi, (uint8_t *)data, len);
 
    // DMA 완료까지 대기 (필요하면 여기서 다른 작업을 하도록 non-blocking으로 바꿀 수 있음)
    while (s_dma_busy)
    {
        __NOP();
    }
    CS_Deselect();
}
 
// HAL이 DMA 전송 완료 시 호출하는 콜백 (weak 함수 override)
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == s_hspi->Instance)
    {
        s_dma_busy = false;
    }
}
 
static void HardwareReset(void)
{
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(150); // 데이터시트 권장 -> 리셋 후 120ms 이상 대기 후 명령 전송
}

void ILI9341_Init(SPI_HandleTypeDef *hspi)
{
    s_hspi = hspi;
 
    CS_Deselect();
    HardwareReset();
 
    WriteCommand(CMD_SWRESET);
    HAL_Delay(5);
 
    // Power control A
    WriteCommand(CMD_PWCTRA);
    WriteData((uint8_t[]){0x39, 0x2C, 0x00, 0x34, 0x02}, 5);
 
    // Power control B
    WriteCommand(CMD_PWCTRB);
    WriteData((uint8_t[]){0x00, 0xC1, 0x30}, 3);
 
    // Driver timing control A
    WriteCommand(CMD_TIMCTRA);
    WriteData((uint8_t[]){0x85, 0x00, 0x78}, 3);
 
    // Driver timing control B
    WriteCommand(CMD_TIMCTRB);
    WriteData((uint8_t[]){0x00, 0x00}, 2);
 
    // Power on sequence control
    WriteCommand(CMD_PWSEQCTRL);
    WriteData((uint8_t[]){0x64, 0x03, 0x12, 0x81}, 4);
 
    // Pump ratio control
    WriteCommand(CMD_PUMPCTRL);
    WriteData8(0x20);
 
    // Power control 1, 2
    WriteCommand(CMD_PWCTR1);
    WriteData8(0x23);
    WriteCommand(CMD_PWCTR2);
    WriteData8(0x10);
 
    // VCOM control 1, 2
    WriteCommand(CMD_VMCTR1);
    WriteData((uint8_t[]){0x3E, 0x28}, 2);
    WriteCommand(CMD_VMCTR2);
    WriteData8(0x86);
 
    // Memory Access Control (기본 방향 - 회전은 ILI9341_SetRotation으로 별도 설정)
    WriteCommand(CMD_MADCTL);
    WriteData8(0x48);
 
    // Pixel Format: 16bit/pixel (RGB565)
    WriteCommand(CMD_PIXFMT);
    WriteData8(0x55);
 
    // Frame Rate Control
    WriteCommand(CMD_FRMCTR1);
    WriteData((uint8_t[]){0x00, 0x18}, 2);
 
    // Display Function Control
    WriteCommand(CMD_DISCTRL);
    WriteData((uint8_t[]){0x08, 0x82, 0x27}, 3);
 
    // 3Gamma Function Disable
    WriteCommand(CMD_EN3GAM);
    WriteData8(0x00);
 
    // Gamma curve selected
    WriteCommand(CMD_GAMMASET);
    WriteData8(0x01);
 
    // Positive/Negative Gamma Correction
    WriteCommand(CMD_GMCTRP1);
    WriteData((uint8_t[]){0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,
                           0x37,0x07,0x10,0x03,0x0E,0x09,0x00}, 15);
    WriteCommand(CMD_GMCTRN1);
    WriteData((uint8_t[]){0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,
                           0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F}, 15);
 
    WriteCommand(CMD_SLPOUT); // Sleep Out
    HAL_Delay(120); // 데이터시트 필수 대기시간
 
    WriteCommand(CMD_DISPON); // Display ON
    HAL_Delay(20);
}
 
/* ID 검증 오픈 소스 -> 제대로 동작 안함 */
bool ILI9341_VerifyID(uint8_t id_out[4])
{
    uint8_t tx[4] = {CMD_RDDID, 0x00, 0x00, 0x00};
    uint8_t rx[4] = {0};
 
    CS_Select();
    DC_Command();
    // 커맨드 바이트는 DC=Command 상태로 1바이트 전송
    HAL_SPI_Transmit(s_hspi, &tx[0], 1, HAL_MAX_DELAY);
    // 이후 더미 바이트 3개를 DC=Data 상태로 보내며 응답을 수신
    DC_Data();
    HAL_SPI_TransmitReceive(s_hspi, &tx[1], &rx[1], 3, HAL_MAX_DELAY);
    CS_Deselect();
 
    id_out[0] = rx[0];
    id_out[1] = rx[1]; // dummy byte (보통 0x00)
    id_out[2] = rx[2]; // manufacturer ID -> 0x93 이어야 정상
    id_out[3] = rx[3]; // driver version/ID -> 0x41 근처
 
    // ILI9341 정상 응답: rx[2] == 0x93 && rx[3] == 0x41
    return (rx[2] == 0x93 && rx[3] == 0x41);
}

void ILI9341_SetRotation(uint8_t rotation)
{
    uint8_t madctl;
    switch (rotation % 4)
    {
        case 0: madctl = 0x48; break; // Portrait
        case 1: madctl = 0x28; break; // Landscape
        case 2: madctl = 0x88; break; // Portrait (뒤집힘)
        case 3: madctl = 0xE8; break; // Landscape (뒤집힘)
        default: madctl = 0x48; break;
    }
    WriteCommand(CMD_MADCTL);
    WriteData8(madctl);
}
 
void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t col_data[4] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF };
    uint8_t row_data[4] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF };
 
    WriteCommand(CMD_CASET);
    WriteData(col_data, 4);
 
    WriteCommand(CMD_PASET);
    WriteData(row_data, 4);
 
    WriteCommand(CMD_RAMWR);
}

void ILI9341_DrawBufferBytes(const uint8_t *data, uint32_t len)
{
    // 이미 [hi, lo, hi, lo, ...] 순서로 정렬된 raw 바이트를 그대로 전송
    WriteDataDMA(data, len);
}

void ILI9341_DrawBuffer(const uint16_t *buf, uint32_t pixel_count)
{
    // RGB565는 2바이트/픽셀, 빅엔디안 순서로 보내야 함
    // buf가 이미 바이트 스왑된 상태라면 그대로 DMA, 아니라면 여기서 바이트 스왑 필요
    WriteDataDMA((const uint8_t *)buf, pixel_count * 2);
}

void ILI9341_FillScreen(uint16_t color)
{
    static uint8_t line_buf[ILI9341_WIDTH * 2]; // 한 줄(row) 분량 버퍼, 정적 할당으로 스택 절약
 
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    for (uint16_t i = 0; i < ILI9341_WIDTH; i++)
    {
        line_buf[i * 2]     = hi;
        line_buf[i * 2 + 1] = lo;
    }
 
    ILI9341_SetWindow(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
 
    for (uint16_t row = 0; row < ILI9341_HEIGHT; row++)
    {
        WriteDataDMA(line_buf, ILI9341_WIDTH * 2);
    }
}

void ILI9341_DrawImageFromArray(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
    ILI9341_SetWindow(x, y, x + w - 1, y + h - 1);
    ILI9341_DrawBufferBytes((const uint8_t *)data, (uint32_t)w * h * 2);
}

void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (w == 0 || h == 0) return;

    ILI9341_SetWindow(x, y, x + w - 1, y + h - 1);

    uint16_t color_le = ((color & 0xFF) << 8) | (color >> 8);

    static uint16_t line_buf[ILI9341_WIDTH]; // 한 줄 분량만 재사용 (RAM 절약)
    for (uint16_t i = 0; i < w; i++) line_buf[i] = color_le;

    for (uint16_t row = 0; row < h; row++)
    {
        ILI9341_DrawBufferBytes((uint8_t*)line_buf, (uint32_t)w * 2);
    }
}

void Face_UpdatePosition(int16_t new_x, int16_t new_y, const uint16_t* img)
{
    if (new_x == last_x && new_y == last_y) return;

    int16_t dx = new_x - last_x;
    int16_t dy = new_y - last_y;

    if (last_x < -FACE_WIDTH || abs(dx) >= FACE_WIDTH || abs(dy) >= FACE_HEIGHT)
    {
        if (last_x >= 0) {
            ILI9341_FillRect(last_x, last_y, FACE_WIDTH, FACE_HEIGHT, BG_COLOR);
        }
    }
    else
    {
        if (dx != 0)
        {
            uint16_t strip_w = abs(dx);
            uint16_t strip_x = (dx > 0) ? last_x : (last_x + FACE_WIDTH - strip_w);
            ILI9341_FillRect(strip_x, last_y, strip_w, FACE_HEIGHT, BG_COLOR);
        }

        if (dy != 0)
        {
            uint16_t strip_h = abs(dy);
            uint16_t strip_y = (dy > 0) ? last_y : (last_y + FACE_HEIGHT - strip_h);

            uint16_t overlap_x = (dx > 0) ? (last_x + dx) : last_x;
            uint16_t strip_w2  = FACE_WIDTH - abs(dx);

            ILI9341_FillRect(overlap_x, strip_y, strip_w2, strip_h, BG_COLOR);
        }
    }

    ILI9341_DrawImageFromArray(new_x, new_y, FACE_WIDTH, FACE_HEIGHT, img);

    last_x = new_x;
    last_y = new_y;
}