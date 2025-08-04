/*
 * gfx.c
 *
 *  Created on: Jul 27, 2025
 *      Author: zac23
 */

#include <stdint.h>
#include <ili9341_driver.h>
#include <ili9341_gfx.h>



void ILI9341_RGB565WritePixel(uint32_t i , uint8_t r, uint8_t g, uint8_t b, uint8_t* frameBuffer){
	uint16_t data = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
	uint8_t highByte = data>>8;
	uint8_t lowByte = data&0xFF;
	frameBuffer[i*2] = highByte;
	frameBuffer[(i*2)+1] = lowByte;



}
void ILI9341_HexWritePixel(uint32_t i, uint16_t color, uint8_t* frameBuffer){
	uint8_t highByte = color >>8;
	uint8_t lowByte = color&0xFF;
	frameBuffer[i*2] = highByte;
	frameBuffer[(i*2)+1] = lowByte;


}

void ILI9341_StripeTest( uint8_t rowCounter) {

    uint8_t r255 = 255;
    uint8_t g255 = 0;
    uint8_t b255 = 0;
    uint8_t frameBuffer[153600];


    for(uint32_t i = 0; i < 76800; i++) {
    	if(i%1920==0){
    		rowCounter++;
    		switch(rowCounter%3){
    		case 0:
    			r255 = 255;
    			g255 = 127;
    			b255 = 80;
    			break;
    		case 1:
    			r255 = 46;
    			g255 = 139;
    			b255 = 87;
    			break;
    		case 2:
    			r255 = 255;
    			g255 = 245;
    			b255 = 238;
    			break;
    		default:
    			break;
    		}

    	}
    	//ILI9341_WriteData16(color);
        ILI9341_RGB565WritePixel(i, r255,g255,b255, frameBuffer);

    }// 240 * 320 pixels
    TransmitFrame(239, 319,&frameBuffer);

}

void ILI9341_RGB565FillScreen(uint16_t color){
	uint8_t frameBuffer[153600];
	for (uint32_t i = 0; i<76800;i++){
		ILI9341_HexWritePixel(i,color,frameBuffer);
	}
	TransmitFrame(239,319,&frameBuffer);


}

void ILI9341_Test(void){
	ILI9341_RGB565FillScreen(COLOR_RED);
	ILI9341_RGB565FillScreen(COLOR_GREEN);
	ILI9341_RGB565FillScreen(COLOR_BLUE);
	ILI9341_RGB565FillScreen(COLOR_BLACK);
	ILI9341_RGB565FillScreen(COLOR_WHITE);
	uint8_t i = 0;
	while (i<10){
		ILI9341_StripeTest(i);
		i++;

	}
	ILI9341_RGB565FillScreen(COLOR_WHITE);



}
