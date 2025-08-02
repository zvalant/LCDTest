/*
 * ili9341_driver.c
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */
#include "ili9341_driver.h"
#include "ili9341_hal.h"
#include <stdbool.h>

extern SPI_HandleTypeDef hspi1;
volatile bool DMA_Transfer_Complete = false;

void ILI9341_WriteCommand(uint8_t cmd) {
    ILI9341_CS_Low();
    ILI9341_DC_Low();  // Command mode
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    ILI9341_CS_High();
}

void ILI9341_WriteData(uint8_t data) {
    ILI9341_CS_Low();
    ILI9341_DC_High(); // Data mode
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    ILI9341_CS_High();
}

void ILI9341_WriteData16(uint16_t data) {
    uint8_t buffer[2];
    buffer[0] = data >> 8;    // High byte
    buffer[1] = data & 0xFF;  // Low byte
    ILI9341_CS_Low();
    ILI9341_DC_High();
    HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
}

void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address
    ILI9341_WriteCommand(ILI9341_CASET);
    ILI9341_WriteData16(x0);
    ILI9341_WriteData16(x1);

    // Page address
    ILI9341_WriteCommand(ILI9341_PASET);
    ILI9341_WriteData16(y0);
    ILI9341_WriteData16(y1);

    // Memory write 2 frameBuffer
    ILI9341_WriteCommand(ILI9341_RAMWR);
}

void ILI9341_Init(void) {
    ILI9341_Reset();

    // Software reset
    ILI9341_WriteCommand(ILI9341_SWRESET);
    HAL_Delay(150);

    // Sleep out
    ILI9341_WriteCommand(ILI9341_SLPOUT);
    HAL_Delay(120);
    ILI9341_WriteCommand(ILI9341_PXLFRMT);
    ILI9341_WriteData(0x55);
    ILI9341_WriteCommand(ILI9341_RGBORDER);
    ILI9341_WriteData(0x08);

    // Display on
    ILI9341_WriteCommand(ILI9341_DISPON);
    HAL_Delay(120);

}



void TransmitFrame(uint16_t height, uint16_t width, uint8_t* frameBuffer){
    int section = 51200;
    int sectionCount = 153600/section;
    ILI9341_SetWindow(0, 0,height, width);
	ILI9341_CS_Low();
	ILI9341_DC_High();
    for (int i = 0; i <sectionCount;i++){

    	HAL_SPI_Transmit_DMA(&hspi1, &frameBuffer[i*section], section);
        while(!DMA_Transfer_Complete);
        DMA_Transfer_Complete = false;
    }

	ILI9341_CS_High();


}
void Test_SPI_Data(void) {
    uint8_t test_data = 0xFF;

    for(int i = 0; i < 20; i++) {
        ILI9341_CS_Low();
        ILI9341_DC_High();
        HAL_SPI_Transmit(&hspi1, &test_data, 1, HAL_MAX_DELAY);
        ILI9341_CS_High();
        HAL_Delay(200);
    }
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if(hspi->Instance == SPI1) {
    	DMA_Transfer_Complete = true;
    }
}
