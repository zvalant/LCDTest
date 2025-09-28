/*
 * gfx.h
 *
 *  Created on: Jul 27, 2025
 *      Author: zac23
 */

#ifndef INC_ILI9341_GFX_H_
#define INC_ILI9341_GFX_H_

#include <stdint.h>
#include <stdbool.h>
#include "ili9341_driver.h"

#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_BLACK   0x0000
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F


typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

} ILI9341_Rect_t;

typedef struct {
	uint8_t data[ILI9341_FRAME_BUFFER_SIZE_BYTES];
	bool dirty;
	uint16_t width;
	uint16_t height;

} ILI9341_GFX_FrameBuffer_t;

typedef enum {
	ILI9341_GFX_OK = 0,
	ILI9341_GFX_GENERAL_ERROR,
	ILI9341_GFX_INVALID_PARAMETER,
	ILI9341_GFX_BOUNDRY_ERROR,
} ILI9341_GFX_Result_t;

ILI9341_GFX_Result_t ILI9341_GFX_FillScreen(ILI9341_GFX_FrameBuffer_t *fb,
		uint16_t color);
ILI9341_GFX_Result_t ILI9341_GFX_Clear(ILI9341_GFX_FrameBuffer_t *fb);

#endif /* INC_ILI9341_GFX_H_ */
