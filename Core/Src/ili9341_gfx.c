/*
 * gfx.c
 *
 *  Created on: Jul 27, 2025
 *      Author: zac23
 */

#include <stdint.h>
#include <stdbool.h>
#include <ili9341_driver.h>
#include <ili9341_gfx.h>
#include <string.h>
#include "cmsis_os.h"


uint32_t GetBufferIndex(uint16_t x, uint16_t y, ILI9341_GFX_FrameBuffer_t *fb) {
	return (uint32_t) (y * fb->width + x) * ILI9341_BYTES_PER_PIXEL;
}

bool ValidCoordinates(uint16_t x, uint16_t y, ILI9341_GFX_FrameBuffer_t *fb) {
	return (x < fb->width && y < fb->height);
}

ILI9341_GFX_Result_t ILI9341_GFX_Init(ILI9341_GFX_FrameBuffer_t *fb) {
	if (fb == NULL) {
		return ILI9341_GFX_INVALID_PARAMETER;

	}
	memset(fb->data, 0, ILI9341_FRAME_BUFFER_SIZE_BYTES);
	fb->dirty = false;
	fb->width = ILI9341_WIDTH_PIXELS;
	fb->height = ILI9341_HEIGHT_PIXELS;

	return ILI9341_GFX_OK;

}


ILI9341_GFX_Result_t ILI9341_GFX_Clear(ILI9341_GFX_FrameBuffer_t *fb) {
	uint16_t color = COLOR_WHITE;
	ILI9341_GFX_Result_t clearResult = ILI9341_GFX_FillScreen(fb, color);

	return clearResult;

}

ILI9341_GFX_Result_t ILI9341_GFX_FillScreen(ILI9341_GFX_FrameBuffer_t *fb,
		uint16_t color) {
	if (fb == NULL) {
		return ILI9341_GFX_INVALID_PARAMETER;
	}
	uint8_t colorHigh = (color >> 8);
	uint8_t colorLow = (color & 0xFF);
	for (uint32_t i = 0; i < fb->width * fb->height; i++) {
		fb->data[i * 2] = colorHigh;
		fb->data[(i * 2) + 1] = colorLow;

	}
	ILI9341_GFX_Result_t result = ILI9341_Transmit_Frame(fb->data);

	fb->dirty = false;
	return result;

}

ILI9341_GFX_Result_t ILI9341_GFX_FillScreen1(ILI9341_GFX_FrameBuffer_t *fb,
		uint16_t color){
	if (fb == NULL) {
			return ILI9341_GFX_INVALID_PARAMETER;
		}
		uint8_t colorHigh = (color >> 8);
		uint8_t colorLow = (color & 0xFF);
		for (uint32_t i = 0; i < fb->width * fb->height; i++) {
			fb->data[i * 2] = colorHigh;
			fb->data[(i * 2) + 1] = colorLow;

		}

		fb->dirty = false;
		return ILI9341_GFX_OK;
}
ILI9341_GFX_Result_t ILI9341_GFX_SetPixel(ILI9341_GFX_FrameBuffer_t *fb,
		uint16_t x, uint16_t y, uint16_t color) {
	ILI9341_GFX_Result_t result = ILI9341_GFX_OK;
	uint8_t colorHigh = (color >> 8);
	uint8_t colorLow = (color & 0xFF);
	uint32_t Idx = 0;
	if (ValidCoordinates(x, y, fb) == false) {
		return ILI9341_GFX_BOUNDRY_ERROR;
	}
	Idx = GetBufferIndex(x, y, fb);
	fb->data[Idx] = colorHigh;
	fb->data[Idx + 1] = colorLow;
	fb->dirty = true;
	return result;
}

ILI9341_GFX_Result_t ILI9341_StripeTest(ILI9341_GFX_FrameBuffer_t *fb,
		uint8_t rowCounter) {
	uint16_t stripeWidth = 10 * fb->width;
	uint16_t color = COLOR_WHITE; //set to default color

	for (uint32_t i = 0; i < fb->width * fb->height; i++) {
		if (i % stripeWidth == 0) {
			rowCounter++;
			switch (rowCounter % 4) {
			case 0:
				color = COLOR_BLUE;
				break;
			case 1:
				color = COLOR_CYAN;
				break;
			case 2:
				color = COLOR_MAGENTA;
				break;
			case 3:
				color = COLOR_RED;
				break;
			default:
				break;
			}

		}
		ILI9341_GFX_SetPixel(fb, i % fb->width, i / fb->width, color);

	}
	ILI9341_GFX_Result_t frameResult = ILI9341_Transmit_Frame(fb->data);
	return frameResult;
}

ILI9341_GFX_Result_t ILI9341_WriteStripe(ILI9341_GFX_FrameBuffer_t *fb, uint16_t rowCounter){
	uint16_t stripeWidth = 10 * fb->width;
	uint16_t color = COLOR_WHITE; //set to default color

	for (uint32_t i = 0; i < fb->width * fb->height; i++) {
		if (i % stripeWidth == 0) {
			rowCounter++;
			switch (rowCounter % 4) {
			case 0:
				color = COLOR_BLUE;
				break;
			case 1:
				color = COLOR_CYAN;
				break;
			case 2:
				color = COLOR_MAGENTA;
				break;
			case 3:
				color = COLOR_RED;
				break;
			default:
				break;
			}

		}
		ILI9341_GFX_SetPixel(fb, i % fb->width, i / fb->width, color);
	}
	return ILI9341_GFX_OK;

}

ILI9341_GFX_Result_t ILI9341_Test(ILI9341_GFX_FrameBuffer_t *fb) {
	ILI9341_GFX_Result_t result;
	uint16_t testColorBuffer[5] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE,
	COLOR_BLACK, COLOR_CYAN };
	for (int i = 0; i < sizeof(testColorBuffer) / sizeof(testColorBuffer[0]);
			i++) {
		if ((result = ILI9341_GFX_FillScreen(fb, testColorBuffer[i]))
				!= ILI9341_GFX_OK)
			return result;
		HAL_Delay(100);
	}
	result = ILI9341_GFX_Clear(fb);
	uint8_t i = 0;
	while (i<10) {
		ILI9341_StripeTest(fb, i);
		i++;

	}
	result = ILI9341_GFX_Clear(fb);
	return result;

}

