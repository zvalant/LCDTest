/*
 * ili9341_driver.h
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */

#ifndef INC_ILI9341_DRIVER_H_
#define INC_ILI9341_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
//ILI9341 SPI Config
#define ILI9341_SPI_HANDLE hspi1
#define ILI9341_SPI_TIMEOUT_MS 1000

//ILI9341 HW constants
#define ILI9341_WIDTH_PIXELS 240
#define ILI9341_HEIGHT_PIXELS 320
//ILI9341 duration startup constants
#define ILI9341_RESET_STARTUP_DURATION_MS 150
#define ILI9341_SLEEP_STARTUP_DURATION_MS 120
#define ILI9341_DISPLAY_DELAY_MS 120
//ILI9341 Testing Consts
#define SPI_TEST_COUNT 20
#define TEST_DELAY 200
//ILI9341 Display Config constants

#define ILI9341_PIXEL_FORMAT_VALUE 0X55 //RGB565
#define ILI9341_RGB_ORDER_VALUE 0X08 //sets to bgr format
#define ILI9341_TOTAL_PIXELS (ILI9341_WIDTH_PIXELS*ILI9341_HEIGHT_PIXELS)
#define ILI9341_BYTES_PER_PIXEL 2
#define ILI9341_FRAME_BUFFER_SIZE_BYTES (ILI9341_TOTAL_PIXELS * ILI9341_BYTES_PER_PIXEL)

//DMA Configuration
#define ILI9341_DMA_CHUNK_SIZE_BYTES 51200
#define ILI9341_DMA_CHUNK_COUNT (ILI9341_FRAME_BUFFER_SIZE_BYTES / ILI9341_DMA_CHUNK_SIZE_BYTES)
// ILI9341 Commands

#define ILI9341_SWRESET     0x01
#define ILI9341_RDDID       0x04
#define ILI9341_SLPOUT      0x11
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_PXLFRMT     0x3A
#define ILI9341_RGBORDER    0x36
#define ILI9341_FRAMECTLON  0xB1
#define ILI9341_FRAMECTLOFF 0xB2
#define ILI9341_FRAMECTLP   0xB3

//Status Results
typedef enum {
	ILI9341_DRIVER_OK = 0,
	ILI9341_DRIVER_ERROR,
	ILI9341_DRIVER_WINDOW_ERROR,
	ILI9341_DRIVER_TIMEOUT,
	ILI9341_DRIVER_BUSY,
	ILI9341_DRIVER_COMMAND_ERROR,
	ILI9341_DRIVER_DATA8_ERROR,
	ILI9341_DATA16_ERROR
} ILI9341_Driver_Result_t;

//SPI transmission calls
ILI9341_Driver_Result_t ILI9341_Write_Command(uint8_t cmd);
ILI9341_Driver_Result_t ILI9341_Write_Data8(uint8_t data);
ILI9341_Driver_Result_t ILI9341_Write_Data16(uint16_t data);
void ILI9341_565RGBWritePixel(uint32_t i, uint8_t r, uint8_t g, uint8_t b,
		uint8_t *frameBuffer);
//Initialization sequence
ILI9341_Driver_Result_t ILI9341_Init(void);
// Frame Transmission
ILI9341_Driver_Result_t ILI9341_Transmit_Frame(const uint8_t *frameBuffer);

#endif /* INC_ILI9341_DRIVER_H_ */
