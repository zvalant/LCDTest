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
#include "ili9341_config.h"


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





typedef enum {
	ILI9341_OK = 0,
	ILI9341_ERROR,
	ILI9341_TIMEOUT,
	ILI9341_BUSY
} ILI9341_Result_t;


//SPI transmission calls
ILI9341_Result_t ILI9341_WriteCommand(uint8_t cmd);
ILI9341_Result_t ILI9341_WriteData(uint8_t data);
ILI9341_Result_t ILI9341_WriteData16(uint16_t data);
void ILI9341_565RGBWritePixel(uint32_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t* frameBuffer);
//Initialization sequence
ILI9341_Result_t ILI9341_Init(void);
ILI9341_Result_t TransmitFrame(const uint8_t *frameBuffer);
ILI9341_Result_t TransmitFrameRemainder(const uint8_t* frameBuffer);
ILI9341_Result_t ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);





#endif /* INC_ILI9341_DRIVER_H_ */
