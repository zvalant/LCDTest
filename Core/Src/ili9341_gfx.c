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




ILI9341_GFX_Result_t ILI9341_GFX_Init(ILI9341_GFX_FrameBuffer_t* fb){
	if (fb==NULL){
		return ILI9341_GFX_INVALID_PARAMETER;

	}
	memset(fb->data, 0, ILI9341_FRAME_BUFFER_SIZE);
	fb->dirty = false;
	fb->width = ILI9341_WIDTH;
	fb->height = ILI9341_HEIGHT;

	return ILI9341_GFX_OK;

}

ILI9341_GFX_Result_t ILI9341_GFX_Clear(ILI9341_GFX_FrameBuffer_t* fb){
	uint16_t color = COLOR_WHITE;
	ILI9341_GFX_Result_t clearStatus = ILI9341_GFX_FillScreen(fb,color);

	return clearStatus;


}

ILI9341_GFX_Result_t ILI9341_GFX_FillScreen(ILI9341_GFX_FrameBuffer_t*fb, uint16_t color){
	ILI9341_Result_t fillScreenResult = ILI9341_GFX_OK;
	if (fb==NULL){
		return ILI9341_GFX_INVALID_PARAMETER;
	}
	uint8_t colorHigh = (color>>8);
	uint8_t colorLow = (color&0xFF);
	for (uint32_t i = 0; i < ILI9341_TOTAL_PIXELS;i++){
		fb->data[i*2] = colorHigh;
		fb->data[(i*2)+1] = colorLow;


	}
	 fillScreenResult = TransmitFrame(fb->data);

	fb->dirty = false;
	return fillScreenResult;



}
ILI9341_GFX_Result_t ILI9341_GFX_SetPixel(ILI9341_GFX_FrameBuffer_t*fb, uint16_t x, uint16_t y, uint16_t color){
	ILI9341_Result_t result = ILI9341_GFX_OK;
	uint8_t colorHigh = (color>>8);
	uint8_t colorLow = (color&0xFF);
	if(!ValidCoordinates(x,y)){
		return ILI9341_GFX_BOUNDRY_ERROR;
	}
	uint32_t Idx = GetBufferIndex(x,y);
	fb->data[Idx] = colorHigh;
	fb->data[Idx+1] = colorLow;
	fb->dirty = true;
	return result;
}




void ILI9341_StripeTest(ILI9341_GFX_FrameBuffer_t* fb, uint8_t rowCounter) {
	uint16_t stripeWidth = 10*ILI9341_WIDTH;
	uint16_t color = COLOR_RED;


    for(uint32_t i = 0; i < ILI9341_TOTAL_PIXELS; i++) {
    	if(i%stripeWidth==0){
    		rowCounter++;
    		switch(rowCounter%3){
    		case 0:
    			color = COLOR_RED;
    			break;
    		case 1:
    			color = COLOR_CYAN;

    			break;
    		case 2:
    			color = COLOR_MAGENTA;
    			break;
    		default:
    			break;
    		}

    	}
    	//ILI9341_WriteData16(color);
    	ILI9341_GFX_SetPixel(fb, i%ILI9341_WIDTH, i/ILI9341_WIDTH, color);

    }// 240 * 320 pixels
    ILI9341_Result_t testResponse = TransmitFrame(fb->data);

}



uint32_t GetBufferIndex(uint16_t x, uint16_t y){
	return (y*ILI9341_WIDTH+x)*2;
}

bool ValidCoordinates(uint16_t x, uint16_t y){
	return (x<ILI9341_WIDTH && y< ILI9341_HEIGHT);
}
void ILI9341_Test(ILI9341_GFX_FrameBuffer_t *fb){
	ILI9341_GFX_Result_t result;
	uint16_t testColorBuffer[5] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_BLACK, COLOR_CYAN};
	for (int i = 0; i < sizeof(testColorBuffer)/sizeof(testColorBuffer[0]); i++){
		result = ILI9341_GFX_FillScreen(fb, testColorBuffer[i]);
		HAL_Delay(100);
	}
	result = ILI9341_GFX_Clear(fb);
	uint8_t i = 0;
	while (i<10){
		ILI9341_StripeTest(fb,i);
		i++;

	}
	result = ILI9341_GFX_Clear(fb);


}
